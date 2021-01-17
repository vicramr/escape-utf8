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
