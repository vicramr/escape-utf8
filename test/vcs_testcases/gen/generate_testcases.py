"""
This program is used to create the test cases in the gen directory.
You must run this script with Python 3.
"""

if __name__ == "__main__":
    # control: tests escaping of ASCII control characters
    with open("control", mode="wb") as f:
        # tab is 09, LF is 0A, CR is 0D, space is 20, tilde is 7E, DEL is 7F
        numbytes = f.write(b"foo \x01bar\r\n\x08\x09\x0A\x0B\x0C\x0D\n\x1F\x20\x7E\x7F")
        assert numbytes == 21
    print("Wrote control successfully")

    # holamundo: tests handling of a 2-byte codepoint
    with open("holamundo", mode="w", encoding="utf8", newline='\n') as f:
        numchars = f.write("\u00A1Hola mundo!\n")
        assert numchars == 13
    print("Wrote holamundo successfully")
