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

# 将slave port赋值给master port
system.packet_gen.plp_port.connect(system.chiplet_link.plp_port)
system.packet_gen.packet_num = 10
system.packet_gen.latency  = "1ps"

# 将system添加到root
root.system = system

# 开始模拟
m5.instantiate()

print("开始模拟......")
exit_event = m5.simulate()
print('退出原因: %s' % exit_event.getCause())