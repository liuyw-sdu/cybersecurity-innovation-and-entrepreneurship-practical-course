import struct

class Parser:
    # 初始化
    def __init__(self, raw):
        self.raw = bytes.fromhex(raw)
        self.offset = 0  # 当前解析位置
    # 读出指定长度的字节数据
    def read(self, n):
        data = self.raw[self.offset:self.offset + n]  # 读取n个字节
        self.offset += n  # 更新解析位置
        return data
    # 预览指定长度的字节数据
    def preview(self, n):
        data = self.raw[self.offset:self.offset + n]  # 读取n个字节
        return data
    # 读取32位小端序无符号整数
    def readuint32(self):
        return struct.unpack("<I", self.read(4))[0]
    # 读取64位小端序无符号整数
    def readuint64(self):
        return struct.unpack("<Q", self.read(8))[0]
    # 读取可变长度小端序无符号整数
    def readvarint(self):
        prefix = self.read(1)[0]
        if prefix < 0xfd:
            # 直接表示的8位整数
            return prefix
        elif prefix == 0xfd:
            # 0xfd后为16位整数
            return struct.unpack("<H", self.read(2))[0]
        elif prefix == 0xfe:
            # 0xfe后为32位整数
            return struct.unpack("<I", self.read(4))[0]
        else:
            # 0xff后为64位整数
            return struct.unpack("<Q", self.read(8))[0]