# ProtocolConverter.py
# 协议转换器基类

from m5.params import *
from m5.proxy import *
from m5.SimObject import SimObject

class ProtocolConverter(SimObject):
    type = 'ProtocolConverter'
    cxx_header = "expr/protocol_converter/converters/ProtocolConverter.hh"
    cxx_class = 'gem5::ProtocolConverter'
    
    # 基本参数
    sourceProtocolName = Param.String("PCIe")
    targetProtocolName = Param.String("PCIe")