// synthesis verilog_input_version verilog_2001
module top_module (
    input [3:0] in,
    output reg [1:0] pos  );

always @(*) begin
  case (in)
     4'h1,4'h3,4'h5,4'h7,4'h9,4'hb,4'hd,4'hf,: pos = 0;
     4'h2,4'h6,4'ha,4'he:pos = 1;
     4'h4,4'hc,:pos = 2;
     4'h8:pos = 3;
     default: pos = 0 ;
  endcase
end
endmodule
