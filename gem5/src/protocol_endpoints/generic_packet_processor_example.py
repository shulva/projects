# generic_packet_processor_example.py
# 通用数据包处理器示例配置脚本

import m5
from m5.objects import *
from m5.util import addToPath
import argparse
import sys
import os

# 实例化系统
root = Root(full_system = False)
root.SimpleChipletLink = SimpleChipletLink()


print("----------------------------------------------------")
# 开始模拟
m5.instantiate()
print("开始模拟...")
exit_event = m5.simulate()
print('退出原因: %s' % exit_event.getCause())