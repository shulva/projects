cmake_minimum_required(VERSION 3.28)
project(gem5)

set(CMAKE_CXX_STANDARD 14)

include_directories(src/protocol_converter)
include_directories(src/protocol_converter/converters)
include_directories(src/protocol_converter/link_layer)
include_directories(src/protocol_converter/packet)
include_directories(src/protocol_converter/phy_abstraction)
include_directories(src/protocol_converter/upf)

add_executable(gem5
        src/protocol_converter/converters/CXLConverter.hh
        src/protocol_converter/converters/PCIeConverter.cc
        src/protocol_converter/converters/PCIeConverter.hh
        src/protocol_converter/converters/ProtocolConverter.cc
        src/protocol_converter/converters/ProtocolConverter.hh
        src/protocol_converter/converters/UCIeConverter.hh
        src/protocol_converter/packet/PLP.hh
        src/protocol_converter/phy_abstraction/SimpleChipletLink.cc
        src/protocol_converter/phy_abstraction/SimpleChipletLink.hh
        src/protocol_converter/upf/UPF.hh
)
