######################################################################
#
# File name : tb_crc_compile.do
# Created on: Sat Jun 22 19:36:53 +0800 2024
#
# Auto generated by Vivado for 'behavioral' simulation
#
######################################################################
vlib modelsim_lib/work
vlib modelsim_lib/msim

vlib modelsim_lib/msim/xil_defaultlib

vmap xil_defaultlib modelsim_lib/msim/xil_defaultlib

vlog -64 -incr -work xil_defaultlib  \
"../../../../CRC.srcs/sources_1/new/crc.v" \
"../../../../CRC.srcs/sim_1/new/tb_crc.v" \


# compile glbl module
vlog -work xil_defaultlib "glbl.v"

quit -force
