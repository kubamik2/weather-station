import sys
result = "uint8_t bitmap["
filename = input("filename: ")
# Read a file with raw image bytes
with open(filename, "rb") as f:
    l = []
    byte = 0
    i = 0
    while color := f.read(4):
        r,g,b = color[0], color[1], color[2]
        byte <<= 1
        # If color isn't white then it's black
        if r != 255 or g != 255 or b != 255:
            byte |= 1
        i += 1

        # If whole byte is present then append
        if i == 8:
            l.append(str(byte))
            i = 0
            byte = 0
    result += f"{str(len(l))}] = {{"
    result += f"{",".join(l)}}};"
    print(result)
