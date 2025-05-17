# SimpleChipletLink.py
# 简单芯粒链路物理层抽象模型参数文件

from m5.params import *
from m5.proxy import *
from m5.SimObject import SimObject

class SimpleChipletLink(SimObject):
    type = 'SimpleChipletLink'
    cxx_header = "protocol_converter/phy_abstraction/SimpleChipletLink.hh"
    cxx_class = 'gem5::SimpleChipletLink'

    # 链路参数
    bandwidth = Param.UInt64(16, "链路带宽 (Gbps)")
    baseLatency = Param.Tick(100, "基础延迟 (ps)")
    bitErrorRate = Param.Double(1e-12, "位错误率")
    linkWidth = Param.UInt32(16, "链路宽度 (bits)")
    encodingOverhead = Param.Double(1.0, "编码开销 (如8b/10b为1.25)")
    
    # 协议参数
    protocol = Param.UInt8(0, "默认协议类型 (0:PCIe, 1:CXL, 2:UCIe)")
    
    # 随机数种子
    randomSeed = Param.UInt32(0, "随机数生成器种子")