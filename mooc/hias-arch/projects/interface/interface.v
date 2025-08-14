module commonAlu (
  input wire clk,
  input wire rst,
  input wire data_ready,//要进行运算的数据是否准备就绪的标�?
  input wire [31:0] x, //操作数A
  input wire [31:0] y, //操作数B
  input wire [3:0] ctrl, //功能函数，�?�择执行具体的哪项功�?
  input wire [4:0] save_no_in, //保留站号
  input wire [4:0] rd_rob_in, //结果影响到的ROB
  output reg done,  //运算器操作的完成信号
  output reg [4:0] save_no_out,
  output reg [4:0] rd_rob_out,
  output reg signed [31:0] result//运算器的执行效果
);


  wire [31:0] result_div,result_mul;
  reg [31:0] input_a_div,input_b_div,input_a_mul,input_b_mul;
  
  wire [31:0] test1;
  wire test2;

  wire busy_div;
  wire exception_mul;

  wire done_div,done_mul;

  divider div(input_a_div,input_b_div,data_ready,clk,rst,data_ready,result_div,busy_div);
  multiplier mul(clk,data_ready,rst,input_a_mul,input_b_mul,result_mul,exception_mul,done_mul);

  always @(*) begin
    save_no_out <= save_no_in;
    rd_rob_out <= rd_rob_in;
  end

  always @(*) begin
    if (ctrl == 4'b0010) begin//乘法
      input_a_mul <= x;
      input_b_mul <= y;
      result <=result_mul;
      done <= done_mul;
    end
    else if (ctrl == 4'b0011) begin //除法
      input_a_div <= x;
      input_b_div <= y;
      result <=result_div;
      done <= !busy_div;
    end
  end
  
  // ila_0 ila_inst(.clk(clk),.probe0(result),.probe1(done),.probe2(clk));

endmodule
