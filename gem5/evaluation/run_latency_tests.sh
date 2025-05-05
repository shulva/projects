#!/bin/bash
# 评估脚本 - 运行延迟测试
# 该脚本负责调用 gem5，使用不同的配置参数（如低注入率）
# 来运行模拟，以测量端到端延迟。

GEM5_BUILD_DIR="path/to/your/gem5/build/X86" # 需要修改为你的 gem5 构建目录
GEM5_EXECUTABLE="${GEM5_BUILD_DIR}/gem5.opt"
CONFIG_SCRIPT_DIR="../configs/chiplet_interconnect"

echo "Running Latency Tests..."

# 示例：运行基础拓扑的延迟测试
# $GEM5_EXECUTABLE --outdir=latency_results/base_low_load ${CONFIG_SCRIPT_DIR}/run_base_topology.py --injection-rate=low ... (其他参数)

# ... (后续将添加更多测试命令)

echo "Latency tests complete."