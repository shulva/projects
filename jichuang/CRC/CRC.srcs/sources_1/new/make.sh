iverilog -o wave circular_queue.v
vvp -n wave -lxt2
open -a gtkwave test.vcd
