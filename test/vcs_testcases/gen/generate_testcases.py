"""
This program is used to create the test cases in the gen directory.
You must run this script with Python 3.
"""
from codecs import encode

if __name__ == "__main__":
    # control: tests escaping of ASCII control characters
    with open("control", mode="wb") as f:
        # tab is 09, LF is 0A, CR is 0D, space is 20, tilde is 7E, DEL is 7F
        numbytes = f.write(b"foo \x01bar\r\n\x08\x09\x0A\x0B\x0C\x0D\n\x1F\x20\x7E\x7F")
        assert numbytes == 21
    print("Wrote control successfully")

    # holamundo: tests handling of a 2-byte codepoint
    with open("holamundo", mode="w", encoding="utf8", newline="\n") as f:
        numchars = f.write("\u00A1Hola mundo!\n")
        assert numchars == 13
    print("Wrote holamundo successfully")

    # shortmix: short file with 1, 2, 3, and 4-byte codepoints
    with open("shortmix", mode="w", encoding="utf8", newline="\r\n") as f:
        # 10904 is a Phoenician symbol.
        numchars = f.write("\u2020 \u0007\n\U00010904\uFE18\u042F\n\n")
        assert numchars == 9
    print("Wrote shortmix successfully")

    # 255: malformed file with a single byte of value 255
    with open("255", mode="wb") as f:
        numbytes = f.write(b"\xFF")
        assert numbytes == 1
    print("Wrote 255 successfully")

    # bad4byte: malformed file with a 4-byte codepoint that encodes a value of 0
    with open("bad4byte", mode="wb") as f:
        numbytes = f.write(encode("\U0001F0A1", encoding="utf8"))
        assert numbytes == 4
        numbytes = f.write(b"\xF0\x80\x80\x80foo bar")
        assert numbytes == 11
    print("Wrote bad4byte successfully")

    # truncate: malformed file with a 3-byte codepoint that's missing the third byte
    with open("truncate", mode="wb") as f:
        numbytes = f.write(b"first line\nsecond line")
        assert numbytes == 22
        yap = encode("\u2D52", encoding="utf8")
        numbytes = f.write(yap[:-1])
        assert numbytes == 2
        numbytes = f.write(b"unreachable")
        assert numbytes == 11
    print("Wrote truncate successfully")

    # whitespace: contains all the different kinds of ASCII space chars
    with open("whitespace", mode="wb") as f:
        whitespace = b"Tab:\tfoo\nLF:\nfoo\n\\v:\x0Bfoo\n\\f:\x0Cfoo\nCR:\rfoo\nSpace: foo\n"
        numbytes = f.write(whitespace)
        assert numbytes == 52
    print("Wrote whitespace successfully")

    # len6: contains 4-byte chars whose escaped forms are 6 chars long
    # note: 100000 through 10FFFD are in the private use area, while 10FFFE and 10FFFF
    # are considered "noncharacters". However, consumers of Unicode data should not
    # reject text with these characters.
    # See here: https://en.wikipedia.org/wiki/Private_Use_Areas#Private_Use_Areas
    with open("len6", mode="wb") as f:
        numbytes = f.write(encode("\U000FFFFF\U00100000 \U00100001\U0010FFFD \U0010FFFE\U0010FFFF", encoding="utf8"))
        assert numbytes == 26
    print("wrote len6 successfully")

    # boundary_success: valid UTF-8 file that contains chars at the boundaries
    # of codepoint ranges for 1, 2, 3, and 4-byte chars.
    # Note: text editors appear to have more trouble than usual displaying this file,
    # but it should be 100% valid UTF-8.
    with open("boundary_success", mode="wb") as f:
        numbytes = f.write(encode("\u0000\u007F", encoding="utf8"))
        assert numbytes == 2
        numbytes = f.write(encode("\u0080\u07FF", encoding="utf8"))
        assert numbytes == 4
        numbytes = f.write(encode("\u0800\uFFFF", encoding="utf8"))
        assert numbytes == 6
        numbytes = f.write(encode("\U00010000\U0010FFFF", encoding="utf8"))
        assert numbytes == 8
    print("wrote boundary_success successfully")

    # boundary_fail_2byte: invalid UTF-8 file that contains a 2-byte character
    # with a codepoint of 0x7F. This is 1 less than the valid range for 2-byte
    # UTF-8 characters.
    with open("boundary_fail_2byte", mode="wb") as f:
        numbytes = f.write(b"foo\xC1\xBFbar")
        assert numbytes == 8
    print("wrote boundary_fail_2byte successfully")

    # boundary_fail_3byte: invalid UTF-8 file that contains a 3-byte character
    # encoding a codepoint of 0x123.
    with open("boundary_fail_3byte", mode="wb") as f:
        numbytes = f.write(b"foo\xE0\x84\xA3bar")
        assert numbytes == 9
    print("wrote boundary_fail_3byte successfully")

    # boundary_fail_4byte: invalid UTF-8 file that contains a 4-byte character
    # encoding a codepoint of 0x110000. This is 1 more than the largest valid
    # Unicode codepoint.
    with open("boundary_fail_4byte", mode="wb") as f:
        numbytes = f.write(b"foo\xF4\x90\x80\x80bar")
        assert numbytes == 10
    print("Wrote boundary_fail_4byte successfully")
