from m5.params import *
from m5.proxy import *
from m5.SimObject import SimObject

class PCIeLinkLayer(SimObject):
    type = 'PCIeLinkLayer'
    cxx_header = "expr/protocol_converter/link_layer/PCIeLinkLayer.hh"
    cxx_class = 'gem5::PCIeLinkLayer'

 