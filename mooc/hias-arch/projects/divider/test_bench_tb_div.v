`timescale 1ns / 100ps
module test_bench_tb_div;
  reg clk;
  reg rst;
  reg start;
  reg enable;
  reg [12:0] a;
  reg [23:0] b;
  reg [23:0] c;

  reg [31:0] d;
  reg [31:0] e;
  wire [31:0] f;
  wire busy;
  wire stall;

  initial begin
    rst <= 1'b0;
    #10 rst <= 1'b1;

    clk <= 1'b0;

    while (1) begin
      #5 clk <= ~clk;
    end
  end

  initial begin
    #1000 $finish;
  end

  initial begin
    $dumpfile("wave.vcd");  //生成的vcd文件名称
    $dumpvars(0, test_bench_tb_div);  //tb模块名称
  end

  initial begin
    # 15
    enable <= 1'b1;
    start <= 1'b1;
    d   <= 32'hc396d200;
    e   <= 32'hc0100000;// q = 43061000

/*
    # 10
    d   <= 32'h42e88000;
    e   <= 32'h41780000;// q = 40f00000
*/
  end

  always @(*)  begin
    if (busy == 0) begin
      d   <= 32'h40ae0000;
      e   <= 32'hbec00000;// q = c1680000
    end
  end

  divider di(d,e,start,clk,rst,enable,f,busy);

endmodule
