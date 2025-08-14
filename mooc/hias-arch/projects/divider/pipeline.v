module newton_24(
  input clk,
  input enable, //for pipeline
  input rst,
  input start,
  input [23:0] a,
  input [23:0] b,

  output reg busy,//通过busy暂停流水线的输入
  output [23:0] q
);

  reg [4:0] count;
  wire [7:0] x0 = rom(b[22:19]);

  reg [25:0] reg_x;
  reg [23:0] reg_a;
  reg [23:0] reg_b;

  wire [49:0] bxi = reg_x*reg_b;
  wire [25:0] b_2m = ~bxi[48:23] +1'b1;
  wire [51:0] x52 = reg_x*b_2m;

  // assign stall = start & (count==0) | busy;

  always @ (posedge clk or negedge rst) begin
    if (rst == 0) begin
      count <= 5'b0;
      busy <= 1'b0;
    end
    else begin
      if (start & count == 0) begin
        count <= 5'b1;
        busy <= 1'b1;
      end
      else begin
        if (count == 5'h1) begin
          reg_a <= a;
          reg_b <= b;
          reg_x <= {2'b1,x0,16'b0};
        end
        
        if (count != 0)
          count <= count + 5'b1;

        if (count == 5'h10)
          busy <= 0; // ok for next

        if (count == 5'h10)
          count <= 5'b0;

        if ((count == 5'h06) |(count == 5'h0b) |(count == 5'h10) )// 暂时一次循环5个clk(两个乘法一个加法)
          reg_x <= x52[50:25];

      end
    end
  end

  wire [49:0] axi = reg_x*reg_a;
  //这之间可以考虑流水
  assign q = axi[48:25] + |axi[24:0]; // 24位，没有round的空间了，想要可以在final之前再加
  
// rom -------------------------
  function [7:0] rom;
    input [3:0] b;
    case (b)
      4'h0: rom = 8'hff; 4'h1: rom = 8'hdf;
      4'h2: rom = 8'hc3; 4'h3: rom = 8'haa;
      4'h4: rom = 8'h93; 4'h5: rom = 8'h7f;
      4'h6: rom = 8'h6d; 4'h7: rom = 8'h5c;
      4'h8: rom = 8'h4d; 4'h9: rom = 8'h3f;
      4'ha: rom = 8'h33; 4'hb: rom = 8'h27;
      4'hc: rom = 8'h1c; 4'hd: rom = 8'h12;
      4'he: rom = 8'h08; 4'hf: rom = 8'h00;
    endcase
  endfunction

endmodule


