import parse
import json

class TxParser(parse.Parser):
    # 解析交易数据
    def parsetx(self): 
        tx = {}
        # 版本号
        tx["version"] = self.readuint32()
        # SegWit标记
        segwit = False
        if self.preview(2) == b'\x00\x01':
            segwit = True
            self.read(2)  # 跳过标记字节
        tx["segwit"] = segwit
        # 输入
        vin_count = self.readvarint()  # 输入数量
        tx["vin"] = []
        for _ in range(vin_count):
            vin = {}
            # 输入TXID
            vin["txid"] = self.read(32)[::-1].hex()  # 反转顺序
            # 输入的输出索引
            vin["vout"] = self.readuint32()
            # 输入脚本
            vin["scriptSig"] = self.read(self.readvarint()).hex()
            # 输入序列号
            vin["sequence"] = self.readuint32()
            # 记录输出信息
            tx["vin"].append(vin)
        # 输出
        vout_count = self.readvarint()  # 输出数量
        tx["vout"] = []
        for _ in range(vout_count):
            vout = {}
            # 输出金额
            vout["value_satoshi"] = self.readuint64()
            # 输出脚本
            vout["scriptPubKey"] = self.read(self.readvarint()).hex()
            # 记录输出信息
            tx["vout"].append(vout)
        # 输入的Witness数据
        if segwit:
            for vin in tx["vin"]:  # 对每个输入
                witness_count = self.readvarint()  # Witness元素数量
                vin["txinwitness"] = []
                for _ in range(witness_count):
                    item_len = self.readvarint()  # Witness元素长度
                    item = self.read(item_len)  # Witness元素数据
                    vin["txinwitness"].append(item.hex())
        # 锁定时间
        tx["locktime"] = self.readuint32()
        # 返回解析后交易数据
        return tx

if __name__ == "__main__":
    # 读取原始交易数据
    with open("tx.hex", "r") as f:
        rawtx = f.read().strip()
    # 初始化解析器
    parser = TxParser(rawtx)
    # 解析交易数据
    result = parser.parsetx()
    # 输出解析后交易数据
    print(json.dumps(result, indent=2))