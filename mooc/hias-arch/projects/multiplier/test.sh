#!/bin/bash
echo "开始编译"
iverilog -o wave multiplier.v wallace_tree.v dependency.v booth_24x24.v test_bench_tb_mul.v pipeline.v
echo "编译完成"
vvp -n wave -lxt2
echo "生成波形文件"
cp wave.vcd wave.lxt
echo "查看波形"
gtkwave.exe wave.vcd
