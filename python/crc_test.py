import zlib

data = bytes.fromhex("A29488AA69448885B903EF9BCFB54B5E7FD3513C305D7D0836F36D112AA79043")

crc = zlib.crc32(data) & 0xFFFFFFFF
print(f"CRC32: {crc:08X}")
