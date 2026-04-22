import sys
import struct

bin_file = "New_Application.bin"
c_file = "ota_image.h"
array_name = "ota_image_bin"

def crc32(data):
    crc = 0xFFFFFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 1:
                crc = (crc >> 1) ^ 0xEDB88320
            else:
                crc >>= 1
    return crc ^ 0xFFFFFFFF

with open(bin_file, "rb") as f:
    app_data = f.read()

image_size = len(app_data)
crc = crc32(app_data)
magic = 0xABCDEFAB
version = 1

# Build 16-byte OTA header
header = struct.pack("<IIII", magic, image_size, crc, version)
combined = list(header) + list(app_data)

with open(c_file, "w") as f:
    f.write(f"const unsigned char {array_name}[] = {{\n")
    for i, b in enumerate(combined):
        f.write(f"0x{b:02X}, ")
        if (i + 1) % 12 == 0:
            f.write("\n")
    f.write("\n};\n")
    f.write(f"const unsigned int {array_name}_len = {len(combined)};\n")

print(f"Done: size={image_size}, crc=0x{crc:08X}")