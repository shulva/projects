# PCIeConverter.py
# PCIe协议转换器参数文件

from m5.params import *
from m5.proxy import *
from m5.SimObject import SimObject

class PCIeConverter(SimObject):
    type = 'PCIeConverter'
    cxx_header = "expr/protocol_converter/converters/PCIeConverter.hh"
    cxx_class = 'gem5::PCIeConverter'
    
    # 参数默认值
    targetProtocol = Param.String("CXL", "目标协议名称")