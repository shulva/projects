`timescale 1ns / 100ps
module test_bench_tb_mul;
  reg clk;
  reg clrn;
  reg start;
  reg [12:0] a;
  reg [23:0] b;
  reg [23:0] c;

  reg [31:0] d;
  reg [31:0] e;
  wire [31:0] f;
  wire exception;
  wire done;

  initial begin
    clrn <= 1'b0;
    #10 clrn <= 1'b1;

    clk <= 1'b0;
    start <= 1'b1;

    while (1) begin
      #5 clk <= ~clk;
    end

  end

  initial begin
    #1000 $finish;
  end

  initial begin
    $dumpfile("wave.vcd");  //生成的vcd文件名称
    $dumpvars(0, test_bench_tb_mul);  //tb模块名称
  end

  initial begin

    # 10
    d   <= 32'h7f7fffff;
    e   <= 32'h7f7fffff;

    # 10
    d   <= 32'h43061000;
    e   <= 32'hc0100000;

    # 10
    d   <= 32'hc1680000;
    e   <= 32'hbec00000;

    # 10
    d   <= 32'h40f00000;
    e   <= 32'h41780000;


  end
 
  multiplier m(clk,start,clrn,d,e,f,exception,done);

endmodule
