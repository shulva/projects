from m5.params import *
from m5.proxy import *
from m5.SimObject import SimObject

class LinkLayer(SimObject):
    type = 'LinkLayer'
    cxx_header = "expr/protocol_converter/link_layer/LinkLayer.hh"
    cxx_class = 'gem5::LinkLayer'

    to_converter_buffer_size = Param.Int(32)
    to_phy_buffer_size = Param.Int(32)

    link_to_phy_Port = SlavePort("accept packet from phy")
    link_to_converter_Port = MasterPort("send packet to converters")
    converter_to_link_slaveport = SlavePort("accept packet from protocol")
    link_to_phy_masterport = MasterPort("send packet to phy")
 