# generic_packet_processor_example.py
# 通用数据包处理器示例配置脚本

import m5
from m5.objects import *
from m5.util import addToPath
import argparse

# 添加配置路径
addToPath('../')

# 创建系统
system = System()

# 设置时钟和电压域
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1GHz'
system.clk_domain.voltage_domain = VoltageDomain()

# 创建物理层
system.physical_layer = SimpleChipletLink(
    bandwidth = 16,  # 16 Gbps
    baseLatency = 100,  # 100 ps
    bitErrorRate = 1e-12,
    linkWidth = 16,  # 16 bits
    encodingOverhead = 1.25,  # 8b/10b编码
    protocol = 0,  # PCIe
    randomSeed = 42
)

# 创建链路层
system.link_layer = PCIeLinkLayer(
    rxBufferSize = 64,
    txBufferSize = 64,
    initialRxCredits = 32,
    initialTxCredits = 32
)

# 创建协议转换器
system.protocol_converter = PCIeConverter(
    sourceProtocol = "PCIe",
    targetProtocol = "CXL"
)

# 创建通用数据包处理器
system.packet_processor = GenericPacketProcessor(
    physicalLayer = system.physical_layer,
    linkLayer = system.link_layer,
    protocolConverter = system.protocol_converter
)

# 实例化系统
root = Root(full_system = False, system = system)

# 开始模拟
m5.instantiate()
print("开始模拟...")
exit_event = m5.simulate()
print('退出原因: %s' % exit_event.getCause())