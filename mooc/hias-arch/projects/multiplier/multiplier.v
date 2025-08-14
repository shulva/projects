module multiplier (
    input clk,
    input enable,
    input clrn,
    input [31:0] input_a,
    input [31:0] input_b,
    output [31:0] output_z,
    output exception,
    output reg done
);

  assign exception= 0 ;// no error

  wire r1_sign;
  wire [9:0] r1_exp;
  wire r1_s_is_nan;
  wire r1_s_is_inf;
  wire [22:0] r1_qnan_frac;
  wire [47:0] r1_z_sum;
  wire [47:0] r1_z_carry;

  pipeline_float_mul mul(input_a,input_b,r1_sign,r1_exp,r1_s_is_nan,r1_s_is_inf,r1_qnan_frac,r1_z_sum,r1_z_carry);

  wire o1_sign;
  wire [9:0] o1_exp;
  wire o1_s_is_nan;
  wire o1_s_is_inf;
  wire [22:0] o1_qnan_frac;
  wire [47:0] o1_z_sum;
  wire [47:0] o1_z_carry;

  reg_mul_add reg_1(clrn,enable,clk,r1_sign,r1_exp,r1_s_is_nan,r1_s_is_inf,r1_qnan_frac,r1_z_sum,r1_z_carry,  o1_sign,o1_exp,o1_s_is_nan,o1_s_is_inf,o1_qnan_frac,o1_z_sum,o1_z_carry);

  wire [47:0] temp_sum;
  wire temp_carry;

  cla_48_adder cla(o1_z_sum,o1_z_carry<<1,1'b0,temp_sum,temp_carry);

  wire r2_sign;
  wire [9:0] r2_exp;
  wire r2_s_is_nan;
  wire r2_s_is_inf;
  wire [22:0] r2_qnan_frac;
  wire [47:0] r2_temp_sum;
  reg [2:0] count;

  reg_add_normalize reg_2(clrn,enable,clk,o1_sign,o1_exp,o1_s_is_nan,o1_s_is_inf,o1_qnan_frac,temp_sum,r2_sign,r2_exp,r2_s_is_nan,r2_s_is_inf,r2_qnan_frac,r2_temp_sum);

  float_add_normalize normalize(clk,r2_sign,r2_exp,r2_s_is_nan,r2_s_is_inf,r2_qnan_frac,r2_temp_sum,output_z);

  always @(negedge clrn) begin
    if (clrn == 0)
      count <= 1;
  end

  always @(posedge clk) begin
    if (count != 0)
      count <= count + 5'b1;

    if (count == 2'b11)
      done <= 1'b1;
  end

endmodule
