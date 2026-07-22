import parse
import hashlib
import json

class BlockParser(parse.Parser): 
    # 解析区块头数据
    def parseheader(self):
        header = {}
        # 版本号
        header["version"] = self.readuint32()
        # 前一区块哈希
        header["previousblockhash"] = self.read(32)[::-1].hex()
        # Merkle根哈希
        header["merkleroot"] = self.read(32)[::-1].hex()
        # 时间戳
        header["time"] = self.readuint32()
        # 难度目标
        header["bits"] = self.readuint32()
        # 随机数
        header["nonce"] = self.readuint32()
        # 返回解析后的区块头数据
        return header
    # 解析交易数据
    def parsetxid(self):
        # 版本号
        start_ver = self.offset  # 起始位置
        self.readuint32()
        stop_ver = self.offset  # 结束位置
        # SegWit标记
        segwit = False
        if self.preview(2) == b'\x00\x01':
            segwit = True
            self.read(2)  # 跳过标记字节
        # 输入
        start_io = self.offset  # 起始位置
        vin_count = self.readvarint()  # 输入数量
        for _ in range(vin_count):
            # 输入TXID
            self.read(32)[::-1].hex()  # 反转顺序
            # 输入的输出索引
            self.readuint32()
            # 输入脚本
            self.read(self.readvarint()).hex()
            # 输入序列号
            self.readuint32()
        # 输出
        vout_count = self.readvarint()  # 输出数量
        for _ in range(vout_count):
            # 输出金额
            self.readuint64()
            # 输出脚本
            self.read(self.readvarint()).hex()
        stop_io = self.offset  # 结束位置
        # 输入的Witness数据
        if segwit:
            for _ in range(vin_count):  # 对每个输入
                witness_count = self.readvarint()  # Witness元素数量
                for _ in range(witness_count):
                    self.read(self.readvarint()).hex()
        # 锁定时间
        start_lktm = self.offset  # 起始位置
        self.readuint32()
        stop_lktm = self.offset  # 结束位置
        # 返回解析后交易数据
        txraw = self.raw[start_ver:stop_ver] + \
                self.raw[start_io:stop_io] + \
                self.raw[start_lktm:stop_lktm]  # 获取交易原始数据
        txid = hashlib.sha256(
            hashlib.sha256(txraw).digest()
        ).digest()[::-1].hex()  # 计算TXID
        return txid
    # 解析区块数据
    def parseblock(self):
        block = {}
        # 区块头
        block["header"] = self.parseheader()
        # 交易数量
        tx_count = self.readvarint()
        block["tx_count"] = tx_count
        # 交易
        block["tx"] = []
        for _ in range(tx_count):
            txid = self.parsetxid()
            block["tx"].append(txid)
        # 返回解析后的区块数据
        return block

if __name__ == "__main__":
    # 读取原始区块数据
    with open("block.hex", "r") as f:
        rawblock = f.read().strip()
    # 初始化解析器
    parser = BlockParser(rawblock)
    # 解析区块数据
    result = parser.parseblock()
    # 输出解析后区块数据
    print(json.dumps(result, indent=2))