#!/bin/bash
echo "开始编译"
iverilog -o wave divider.v shift.v test_bench_tb_div.v pipeline.v
echo "编译完成"
vvp -n wave -lxt2
echo "生成波形文件"
cp wave.vcd wave.lxt
echo "查看波形"
gtkwave.exe wave.vcd
