# GenericPacketProcessor.py
# 通用数据包处理器参数文件

from m5.params import *
from m5.proxy import *
from m5.SimObject import SimObject

class GenericPacketProcessor(SimObject):
    type = 'GenericPacketProcessor'
    cxx_header = "expr/protocol_converter/GenericPacketProcessor.hh"
    cxx_class = 'gem5::GenericPacketProcessor'

    # 组件参数
    physical_layer = Param.SimpleChipletLink("物理层组件")
    link_layer = Param.PCIeLinkLayer("链路层组件")
    protocol_converter = Param.ProtocolConverter("协议转换器组件")