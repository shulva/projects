from m5.params import *
from m5.proxy import *
from m5.SimObject import SimObject

class PacketGenerator(SimObject):
    type = 'PacketGenerator'
    cxx_header = "expr/protocol_converter/packet/PacketGenerator.hh"
    cxx_class = 'gem5::PacketGenerator'

    Protocol = Param.UInt8(1, "pcie tlp")
    plp_port = MasterPort("PLP port")

    packet_num = Param.Int(10) #指定的是默认值
    latency = Param.Latency("1ns")