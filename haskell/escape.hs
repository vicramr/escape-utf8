module Main where

import           Control.Exception              ( assert
                                                , finally
                                                )
import           Control.Monad                  ( (>>)
                                                , (>>=)
                                                , foldM
                                                )
import           Data.Bits                      ( (.&.)
                                                , (.|.)
                                                , shiftL
                                                )
import           Data.Char                      ( chr
                                                , ord
                                                )
import           Data.List                      ( foldl' )
import qualified System.Console.GetOpt         as GetOpt
import           System.Environment             ( getArgs )
import qualified System.Exit
import qualified System.IO
import           System.IO                      ( hPutStr
                                                , hPutStrLn
                                                , stderr
                                                )
import           System.IO.Error                ( tryIOError )
import           Text.Printf                    ( hPrintf
                                                , printf
                                                )


-- BEGIN COMMAND-LINE-HANDLING STUFF
version :: String
version = "1.0.0"

helpStr :: String
helpStr =
  "escape-utf8: Transform UTF-8 text to a representation in ASCII.\n\
  \For detailed information please see the README.\n\n\
  \usage:\n\
  \  escape [INPUTFILE] [-o OUTPUTFILE]\n\
  \  escape -h | --help\n\
  \  escape -v | --version\n\n\
  \argument:\n\
  \  INPUTFILE: Path to the input file. If omitted, input will be read from stdin.\n\n\
  \options:"

-- Which flag the user has given
data Flag = Help | Version | OutputFile !String deriving (Eq)

-- Option descriptions
optionDescriptions :: [GetOpt.OptDescr Flag]
optionDescriptions =
  [ GetOpt.Option "h" ["help"] (GetOpt.NoArg Help) "Show this help message."
  , GetOpt.Option "v" ["version"] (GetOpt.NoArg Version) "Show version."
  , GetOpt.Option
    "o"
    ["output"]
    (GetOpt.ReqArg OutputFile "OUTPUTFILE")
    "Path to the file to write output to. If this option is omitted, the output will be printed to stdout."
  ]

-- parseArgs takes as input the command-line args and returns:
--   Left True = exit now with a success exit code
--   Left False = exit now with a failure exit code
--   For Right, the first Maybe is the input filename and the second Maybe is the output filename.
--   If either is missing then that indicates it was not given on the command line.
-- The return value is an IO action because parseArgs may print messages to stdout/stderr.
parseArgs :: [String] -> IO (Either Bool (Maybe String, Maybe String))
parseArgs argv =
  let
    triple = GetOpt.getOpt GetOpt.Permute optionDescriptions argv
    -- handleGoodCmdline handles the args in the case of no parse error
    handleGoodCmdline
      :: [Flag] -> [String] -> IO (Either Bool (Maybe String, Maybe String))
    handleGoodCmdline flags nonOptions = case () of
      _  -- Here we are using a case expression with guards
        | elem Help flags -> putStr
            (GetOpt.usageInfo helpStr optionDescriptions)
        >> return (Left True)
        | elem Version flags -> putStrLn ("escape-utf8 version " ++ version)
        >> return (Left True)
        | otherwise -> if length nonOptions > 1
          then
            hPutStrLn
                stderr
                "Error: too many arguments. Use the --help option for usage instructions."
              >> return (Left False)
          else case (flags, nonOptions) of  -- This is the success case where we continue with the program.
            ((OutputFile outfile) : _, [infile]) ->
              return (Right (Just infile, Just outfile))  -- We just take the first output file given
            ((OutputFile outfile) : _, []) ->
              return (Right (Nothing, Just outfile))
            ([], [infile]) -> return (Right (Just infile, Nothing))
            _              -> assert (null flags && null nonOptions)
                                     (return (Right (Nothing, Nothing)))
    -- handleBadCmdline takes in a list of strings of error messages and prints them
    handleBadCmdline
      :: [String] -> IO (Either Bool (Maybe String, Maybe String))
    handleBadCmdline errors =
      foldl'
          (\acc msg -> acc >> hPutStr stderr msg)
          (hPutStrLn
            stderr
            "Error: invalid usage. Use the --help option for usage instructions."
          )
          errors
        >> return (Left False)
  in
    case triple of
      (flags, nonOptions, []    ) -> handleGoodCmdline flags nonOptions
      (_    , _         , errors) -> handleBadCmdline errors
-- END COMMAND-LINE-HANDLING STUFF


-- getHandles takes the input filename and output filename as input and attempts
-- to open both files. If there is an error, this function prints an error message
-- and returns Nothing. Otherwise this function returns Just (inputhandle, outputhandle).
getHandles
  :: Maybe String
  -> Maybe String
  -> IO (Maybe (System.IO.Handle, System.IO.Handle))
getHandles maybeIn maybeOut =
  let
    -- stdHnd is either stdin or stdout
    -- fstring is a string for printf that should have a single %s for the filename
    -- mode is the IOMode that would be used for opening the file
    -- maybeFilename is either maybeIn or maybeOut
    -- Returns either an error message or a ready-to-use handle
    attemptSetup
      :: System.IO.Handle
      -> String
      -> System.IO.IOMode
      -> Maybe String
      -> IO (Either String System.IO.Handle)
    attemptSetup stdHnd fstring mode maybeFilename = case maybeFilename of
      Just filename -> do
        tryResult <- tryIOError (System.IO.openBinaryFile filename mode)
        case tryResult of
          Right hnd -> return (Right hnd)
          _         -> return (Left (printf fstring filename))
      _ -> do
        tryResult <- tryIOError (System.IO.hSetBinaryMode stdHnd True)
        case tryResult of
          Right _ -> return (Right stdHnd)
          _ ->
            return (Left "Error when setting stdin or stdout to binary mode")
  in
    do
-- TODO is there a better way to structure this?
-- Also, output file is still created even if input file fails to open
      inHndOrError <- attemptSetup
        System.IO.stdin
        "Failed to open input file \"%s\". Exiting now."
        System.IO.ReadMode
        maybeIn
      outHndOrError <- attemptSetup
        System.IO.stdout
        "Failed to open output file \"%s\". Exiting now."
        System.IO.WriteMode
        maybeOut
      case inHndOrError of
        Left  errMsg -> hPutStrLn stderr errMsg >> return Nothing
        Right inHnd  -> case outHndOrError of
          Left  errMsg -> hPutStrLn stderr errMsg >> return Nothing
          Right outHnd -> return (Just (inHnd, outHnd))


-- BEGIN BUSINESS LOGIC

-- The State type holds the current state we're in as we're parsing the input stream.
-- We can have 0, 1, 2, or 3 bytes in the buffer. 0 means we are at the start
-- of a new Unicode character, otherwise we are in the middle of reading one.
-- If there is at least one byte in the buffer then we must store:
-- a) the number of bytes read so far, b) the decoded char so far, and
-- c) the number of bytes in this character, as determined from the first byte.
data State = Start | Middle !Int !Int !Int

-- Byte is just a wrapper for an Int that ensure it is in the valid range
newtype Byte = Byte Int
makeByte :: Int -> Byte
makeByte i = Byte (assert (0 <= i && i <= 255) i)

-- businessLogic takes in the input and output handles and does the actual
-- heavy lifting: it reads data from input, escapes it, and writes to output.
-- If this succeeds then businessLogic returns True. If there is some error
-- (e.g. a malformed file or an IOError) then businessLogic prints an error
-- message and returns False.
-- This function catches all IOErrors.
-- This function is not responsible for closing file handles.
businessLogic :: System.IO.Handle -> System.IO.Handle -> IO Bool
businessLogic inHnd outHnd =
  let
    malformedMsg = "The given text is not valid UTF-8 text. Exiting now."

    -- printCodepoint prints a decoded codepoint to outHnd, escaping
    -- it if necessary. This may throw an IOError.
    printCodepoint :: Int -> IO ()
    printCodepoint c = if (32 <= c && c <= 126) || c == 9 || c == 10 || c == 13
      then System.IO.hPutChar outHnd (chr c)  -- Printable ASCII
      else hPrintf outHnd "\\u'%04X'" c


    -- Given the first byte of a multibyte character,
    -- return the number of bytes, or Nothing if the byte is malformed
    getLen :: Byte -> Maybe Int
    getLen (Byte b) = case () of
      _ | b .&. 0xE0 == 0xC0 -> Just 2
        | b .&. 0xF0 == 0xE0 -> Just 3
        | b .&. 0xF8 == 0xF0 -> Just 4
        | otherwise          -> Nothing

    -- Given the first byte in a multi-byte char, mask out the irrelevant bits
    maskFirstByte :: Byte -> Int
    maskFirstByte (Byte b) = case getLen (Byte b) of
      Just 2 -> b .&. 0x1F
      Just 3 -> b .&. 0x0F
      Just 4 -> b .&. 0x07
      _      -> error "Internal error: invalid byte given to maskFirstByte"

    -- Given byte 2/3/4 (i.e. an internal byte in the character),
    -- return whether it starts with bits 10.
    isValidInternal :: Byte -> Bool
    isValidInternal (Byte b) = (b .&. 0xC0) == 0x80

    -- Given a codepoint (decodedChar) and the number of bytes in the character,
    -- return whether the codepoint is in the valid range.
    inRange :: Int -> Int -> Bool -- (codepoint, len) if it's valid in the range
    inRange decodedChar charLen = case charLen of
      2 -> 0x80 <= decodedChar && decodedChar <= 0x7FF
      3 -> 0x800 <= decodedChar && decodedChar <= 0xFFFF
      4 -> 0x10000 <= decodedChar && decodedChar <= 0x10FFFF
      _ -> error "Internal error: invalid charLen given to inRange"

    -- transition moves the state machine to the next state and potentially
    -- outputs some characters to outHnd
    transition :: Either String State -> Byte -> IO (Either String State)
    transition x (Byte b) = case x of
      Left  s     -> return (Left s)
      Right Start -> if b <= 127
        then printCodepoint b >> return (Right Start)
        else case getLen (makeByte b) of
          (Just len) ->
            return (Right (Middle 1 (maskFirstByte (makeByte b)) len))
          Nothing -> return (Left malformedMsg)
      Right (Middle numRead decodedChar charLen) ->
        let newDecodedChar = (shiftL decodedChar 6) .|. (b .&. 0x3F)
            newNumRead     = numRead + 1
        in  case () of
              _  -- Using case statement with guards to emulate a multiway if-else
                | not (isValidInternal (makeByte b)) -> return
                  (Left malformedMsg)
                | newNumRead == charLen -> if inRange newDecodedChar charLen
                  then printCodepoint newDecodedChar >> return (Right Start)
                  else return (Left malformedMsg)
                | otherwise -> assert
                  (newNumRead < charLen)
                  (return (Right (Middle newNumRead newDecodedChar charLen)))

    -- readAndEscape takes in the contents of the input stream.
    -- It returns either an error message or the final State.
    -- readAndEscape catches any IOErrors due to writing to outHnd
    -- and instead returns Left "errormsg".
    readAndEscape :: String -> IO (Either String State)
    readAndEscape chars = do
      tryResult <- tryIOError
        (foldM transition (Right Start) (map (makeByte . ord) chars))
      return
        (case tryResult of
          Right result -> result
          _ -> Left "There was a fatal error when trying to write the output."
        )
  in
    do
      -- hGetContents can throw an IOError
      tryResult <- tryIOError (System.IO.hGetContents inHnd >>= readAndEscape)
      case tryResult of
        Right (Right Start) -> return True
        Right (Right (Middle _ _ _)) ->
          hPutStrLn stderr malformedMsg >> return False
        Right (Left errorMsg) -> hPutStrLn stderr errorMsg >> return False
        _ -> hPutStrLn stderr "There was an error when reading input."
          >> return False

-- END BUSINESS LOGIC


main :: IO ()
main = do
  argv       <- getArgs
  boolOrPair <- parseArgs argv
  case boolOrPair of
    Left  True                -> System.Exit.exitSuccess
    Left  False               -> System.Exit.exitFailure
    Right (maybeIn, maybeOut) -> do
      maybeHandles <- getHandles maybeIn maybeOut
      case maybeHandles of
        Nothing              -> System.Exit.exitFailure
        Just (inHnd, outHnd) -> do
          isSuccess <- finally
            (businessLogic inHnd outHnd)
            (System.IO.hClose inHnd >> System.IO.hClose outHnd)
          if isSuccess then System.Exit.exitSuccess else System.Exit.exitFailure
