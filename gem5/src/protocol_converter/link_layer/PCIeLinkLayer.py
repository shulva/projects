# PCIeLinkLayer.py
# PCIe链路层参数文件

from m5.params import *
from m5.proxy import *
from m5.SimObject import SimObject

class PCIeLinkLayer(SimObject):
    type = 'PCIeLinkLayer'
    cxx_header = "expr/protocol_converter/link_layer/PCIeLinkLayer.hh"
    cxx_class = 'gem5::PCIeLinkLayer'


    # 缓冲区参数
    rxBufferSize = Param.UInt32(64, "接收缓冲区大小")
    txBufferSize = Param.UInt32(64, "发送缓冲区大小")
    
    # 流控参数
    initialRxCredits = Param.UInt32(32, "初始接收信用")
    initialTxCredits = Param.UInt32(32, "初始发送信用")    
    # 重传参数
    retryTimeout = Param.Latency('1us', "重传超时时间")
    maxRetries = Param.Int(5, "最大重试次数")
    
    # 流控更新参数
    flowControlUpdateInterval = Param.Latency('500ns', "流控更新间隔")
