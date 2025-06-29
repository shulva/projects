# generic_packet_processor_example.py
# 通用数据包处理器示例配置脚本

import m5
from m5.objects import *
import sys
import os

# 实例化系统
root = Root(full_system = False)

# 创建系统时钟
system = System()
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1GHz'
system.clk_domain.voltage_domain = VoltageDomain()

# 添加组件
system.chiplet_link = SimpleChipletLink()
system.packet_gen = PacketGenerator()
system.link = LinkLayer()
system.protocol = ProtocolConverter()
system.link.to_converter_buffer_size = 32
system.link.to_phy_buffer_size = 32

# generator to phy
system.packet_gen.plp_port.connect(system.chiplet_link.phy_port)
system.packet_gen.packet_num = 2
system.packet_gen.latency  = "2ps"

system.chiplet_link.phy_to_link_port.connect(system.link.link_to_phy_Port) # phy to link
system.link.link_to_converter_Port.connect(system.protocol.link_to_protocol_port) # link to protocol
system.protocol.protocol_to_link_port.connect(system.link.converter_to_link_slaveport) #protocol to link
system.link.link_to_phy_masterport.connect(system.chiplet_link.link_to_phy_slaveport)

system.protocol.sourceProtocolName = "PCIe"
system.protocol.targetProtocolName = "PCIe"

# 将system添加到root
root.system = system

# 开始模拟
m5.instantiate()

print("开始模拟......")
exit_event = m5.simulate()
print('退出原因: %s' % exit_event.getCause())