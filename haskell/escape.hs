module Main where

import Control.Exception (assert)
import System.Environment (getArgs)
import qualified System.Console.GetOpt as GetOpt
import Control.Monad ((>>))

-- BEGIN COMMAND-LINE-HANDLING STUFF
version = "x.y.z"

-- Which flag the user has given
data Flag = Help | Version | OutputFile String deriving (Eq)

-- Option descriptions
optionDescriptions :: [GetOpt.OptDescr Flag]
optionDescriptions = [(GetOpt.Option "h" ["help"] (GetOpt.NoArg Help) "Show this help message."),
                      (GetOpt.Option "v" ["version"] (GetOpt.NoArg Version) "Show version."),
                      (GetOpt.Option "o" ["output"] (GetOpt.ReqArg OutputFile "OUTPUTFILE") 
                          "Path to the file to write output to. If this option is omitted, the output will be printed to stdout.")]

-- parseArgs takes as input the command-line args and returns:
--   Left True = exit now with a success exit code
--   Left False = exit now with a failure exit code
--   For Right, the first Maybe is the input filename and the second Maybe is the output filename.
--   If either is missing then that indicates it was not given on the command line.
-- The return value is an IO action because parseArgs may print messages to stdout/stderr.
parseArgs :: [String] -> IO (Either Bool (Maybe String, Maybe String))
parseArgs argv =
    let triple = GetOpt.getOpt GetOpt.Permute optionDescriptions argv
        -- handleGoodCmdline handles the args in the case of no parse error
        handleGoodCmdline :: [Flag] -> [String] -> IO (Either Bool (Maybe String, Maybe String))
        handleGoodCmdline flags nonOptions = case flags of
          _  -- Here we are using a case expression with guards
            | (any (== Help) flags) -> putStrLn (GetOpt.usageInfo "TODO" optionDescriptions) >> return (Left True)
            | (any (== Version) flags) -> putStrLn ("escape-utf8 version " ++ version) >> return (Left True)
            | otherwise -> if (length nonOptions > 1)
                then (putStrLn "Error: too many arguments" >> putStrLn (GetOpt.usageInfo "TODO" optionDescriptions) >> return (Left False))
                else case (flags, nonOptions) of  -- This is the success case where we continue with the program.
                    ((OutputFile outfile):_, [infile]) -> return (Right (Just infile, Just outfile))  -- We just take the first output file given
                    ((OutputFile outfile):_, []) -> return (Right (Nothing, Just outfile))
                    ([], [infile]) -> return (Right (Just infile, Nothing))
                    _ -> assert ((length flags == 0) && (length nonOptions == 0)) (return (Right (Nothing, Nothing)))
        -- handleBadCmdline takes in a list of strings of error messages and prints them
        handleBadCmdline :: [String] -> IO (Either Bool (Maybe String, Maybe String))
        handleBadCmdline errors = (foldl (\acc msg -> acc >> (putStr msg)) (putStrLn "Error: invalid usage") errors) >> return (Left False) -- TODO print usage string
    in case triple of
        (flags, nonOptions, []) -> handleGoodCmdline flags nonOptions
        (_, _, errors) -> handleBadCmdline errors
        _ -> error "Internal error: getOpt did not return a triple"
-- END COMMAND-LINE-HANDLING STUFF


main :: IO ()
main = do
    argv <- getArgs
    boolOrPair <- parseArgs argv
    case boolOrPair of
        Left True -> putStrLn "Exiting with success"
        Left False -> putStrLn "Exiting with failure"
        Right (maybein, maybeout) -> putStrLn (show maybein ++ " " ++ show maybeout)

