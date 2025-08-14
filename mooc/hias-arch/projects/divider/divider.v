module divider(
    input [31:0] input_a,
    input [31:0] input_b,
    input start,
    input clk,
    input rst,
    input enable,

    output [31:0] output_z,
    output busy
);

  wire a_expo_is_ff= &input_a[30:23]; //exp=ff
  wire b_expo_is_ff= &input_b[30:23];

  wire a_expo_is_00= ~|input_a[30:23]; //exp=00
  wire b_expo_is_00= ~|input_b[30:23];

  wire a_frac_is_00 = ~|input_a[22:0];
  wire b_frac_is_00 = ~|input_b[22:0];

  wire a_is_inf=a_expo_is_ff&a_frac_is_00;
  wire b_is_inf=b_expo_is_ff&b_frac_is_00;

  wire a_is_nan=a_expo_is_ff& ~a_frac_is_00;
  wire b_is_nan=b_expo_is_ff& ~b_frac_is_00;

  wire [22:0] qnan_frac = (a_is_nan)?{1'b1,input_a[21:0]}:{1'b1,input_b[21:0]}; //qNan

  wire a_is_0 =a_expo_is_00&a_frac_is_00;
  wire b_is_0 =b_expo_is_00&b_frac_is_00;

  wire s_is_inf=a_is_inf|b_is_inf;
  wire s_is_nan=a_is_nan|(a_is_inf&b_is_0)|b_is_nan|(b_is_inf&a_is_0);

  wire [9:0] exp = {2'b0,input_a[30:23]}-{2'b0,input_b[30:23]}+128-1;

  wire [23:0] temp_frac_a = a_expo_is_00?{input_a[22:0],1'b0}:{1'b1,input_a[22:0]};
  wire [23:0] temp_frac_b = b_expo_is_00?{input_b[22:0],1'b0}:{1'b1,input_b[22:0]};

  wire [23:0] frac_a,frac_b,frac;
  wire [4:0] move_exp_a,move_exp_b;

  shift shift_a(temp_frac_a,frac_a,move_exp_a);
  shift shift_b(temp_frac_b,frac_b,move_exp_b);

  wire sign = input_a[31]^input_b[31];

  wire [9:0] new_exp = exp - move_exp_a + move_exp_b;

  reg r1_sign,r1_a_expo_is_00,r1_a_expo_is_ff,r1_a_frac_is_00,r1_b_expo_is_00,r1_b_expo_is_ff,r1_b_frac_is_00;
  // reg r2_sign,r2_a_expo_is_00,r2_a_expo_is_ff,r2_a_frac_is_00,r2_b_expo_is_00,r2_b_expo_is_ff,r2_b_frac_is_00;
  // reg r3_sign,r3_a_expo_is_00,r3_a_expo_is_ff,r3_a_frac_is_00,r3_b_expo_is_00,r3_b_expo_is_ff,r3_b_frac_is_00;
  reg [9:0] r1_exp,r2_exp,r3_exp;

  always @(negedge rst or posedge clk or negedge busy) begin
    if (rst == 0) begin
      r1_sign <= 0;
      r1_a_expo_is_00 <=0;
      r1_a_expo_is_ff <=0;
      r1_a_frac_is_00 <=0;
      r1_b_expo_is_00 <=0;
      r1_b_expo_is_ff <=0;
      r1_b_frac_is_00 <=0;
      r1_exp <=0;
     /* 
      r2_sign <= 0;
      r2_a_expo_is_00 <=0;
      r2_a_expo_is_ff <=0;
      r2_a_expo_is_ff <=0;
      r2_b_expo_is_00 <=0;
      r2_b_expo_is_ff <=0;
      r2_b_frac_is_00 <=0;
      r2_exp <=0;

      r3_sign <= 0;
      r3_a_expo_is_00 <=0;
      r3_a_expo_is_ff <=0;
      r3_a_expo_is_ff <=0;
      r3_b_expo_is_00 <=0;
      r3_b_expo_is_ff <=0;
      r3_b_frac_is_00 <=0;
      r3_exp <=0;
      */
    end
    else  if (enable & !busy) begin

      r1_sign <= sign;
      r1_a_expo_is_00 <=a_expo_is_00;
      r1_a_expo_is_ff <=a_expo_is_ff;
      r1_a_frac_is_00 <=a_frac_is_00;
      r1_b_expo_is_00 <=b_expo_is_00;
      r1_b_expo_is_ff <=b_expo_is_ff;
      r1_b_frac_is_00 <=b_frac_is_00;
      r1_exp <= new_exp;
    /*  
      r2_sign <= r1_sign;
      r2_a_expo_is_00 <=r1_a_expo_is_00;
      r2_a_expo_is_ff <=r1_a_expo_is_ff;
      r2_a_expo_is_ff <=r1_a_frac_is_00;
      r2_b_expo_is_00 <=r1_b_expo_is_00;
      r2_b_expo_is_ff <=r1_b_expo_is_ff;
      r2_b_frac_is_00 <=r1_b_frac_is_00;
      r2_exp <=r1_exp;

      r3_sign <= r2_sign;
      r3_a_expo_is_00 <=r2_a_expo_is_00;
      r3_a_expo_is_ff <=r2_a_expo_is_ff;
      r3_a_expo_is_ff <=r2_a_frac_is_00;
      r3_b_expo_is_00 <=r2_b_expo_is_00;
      r3_b_expo_is_ff <=r2_b_expo_is_ff;
      r3_b_frac_is_00 <=r2_b_frac_is_00;
      r3_exp <=r2_exp;
      */
    end
  end

  newton_24 frac_div_1(clk,enable,rst,start,frac_a,frac_b,busy,frac);

  wire [23:0] frac_1 = frac[23]?frac:{frac[22:0],1'b0}; // a-f24/b-f24 --> 1.xxxx or 0.1xxxxx
  wire [9:0] new_exp_end = frac[23]?r1_exp:r1_exp-1;

  reg [9:0] exp_end;
  reg [23:0] frac_end;

  always @(*) begin
    if (new_exp_end[9]) begin // exp < 0
      exp_end = 0;
      if (frac_1[23]) begin// 1.xxxxx..
        frac_end = frac_1 >> (1-new_exp_end); // to subnormal
      end
      else begin
        frac_end = 0;
      end
    end
    else if (new_exp_end == 0) begin
      exp_end = 0;
      frac_end = {1'b0,frac[23:2],|frac[1:0]}; // to subnormal
    end
    else begin
      if (new_exp_end > 254) begin //inf
        exp_end = 10'hff;
        frac_end = 0;
      end
      else begin
        exp_end = new_exp_end;
        frac_end = frac_1;
      end
    end

  end
// normalize ------------------------------------------------------------

  wire overflow = (exp_end > 255);
  assign output_z = final(overflow,r1_a_expo_is_00,r1_a_expo_is_ff,r1_a_frac_is_00,
    r1_b_expo_is_00,r1_b_expo_is_ff,r1_b_frac_is_00,r1_sign,exp_end[7:0],frac_end[22:0]);

// final -----------------------
// The divideByZero exception shall be signaled if and only if an exact infinite result is defined for an 
// operation on finite operands. The default result of divideByZero shall be an âˆ? correctly signed according
// to the operation:
// For division, when the divisor is zero and the dividend is a finite non-zero number, the sign of the
// infinity is the exclusive OR of the operandsâ€? signs

  function [31:0] final;
    input overflow;
    input a_expo_is_00;
    input a_expo_is_ff;
    input a_frac_is_00;
    input b_expo_is_00;
    input b_expo_is_ff;
    input b_frac_is_00;
    input sign;
    input [7:0] exp;
    input [22:0] frac;

    casex({overflow,a_expo_is_00,a_expo_is_ff,a_frac_is_00,b_expo_is_00,b_expo_is_ff,b_frac_is_00})
      7'b1_x_x_x_x_x_x: final = {sign,8'hff,23'h000000}; //inf

      7'b0_0_1_0_x_x_x: final = {sign,8'hff,qnan_frac}; // Nan / other
      7'b0_0_1_1_0_1_0: final = {sign,8'hff,qnan_frac}; // inf / nan
      7'b0_1_0_0_0_1_0: final = {sign,8'hff,qnan_frac}; // subnormal / nan
      7'b0_1_0_1_0_1_0: final = {sign,8'hff,qnan_frac}; // 0 / nan
      7'b0_0_0_x_0_1_0: final = {sign,8'hff,qnan_frac}; // normal / nan
      7'b0_0_1_1_0_1_1: final = {sign,8'hff,qnan_frac}; // inf / inf
      7'b0_1_0_1_1_0_1: final = {sign,8'hff,qnan_frac}; // 0 / 0 results in nan

      7'b0_1_0_0_0_1_1: final = {sign,31'h00000000}; // subnormal / inf
      7'b0_1_0_1_0_1_1: final = {sign,31'h00000000}; // 0 / inf
      7'b0_0_0_x_0_1_1: final = {sign,31'h00000000}; // normal / inf
      7'b0_1_0_1_1_0_0: final = {sign,31'h00000000}; // 0 / subnormal
      7'b0_1_0_1_0_0_x: final = {sign,31'h00000000}; // 0 / normal

      7'b0_0_1_1_1_0_1: final = {sign,8'hff,23'h000000}; // inf / 0
      7'b0_1_0_0_1_0_1: final = {sign,8'hff,23'h000000}; // subnormal / 0
      7'b0_0_0_x_1_0_1: final = {sign,8'hff,23'h000000}; // normal / 0
      7'b0_0_1_1_1_0_0: final = {sign,8'hff,23'h000000}; // inf / subnormal
      7'b0_0_1_1_0_0_x: final = {sign,8'hff,23'h000000}; // inf / normal

      7'b0_1_0_0_1_0_0: final = {sign,exp,frac}; // subnormal / subnormal
      7'b0_0_0_x_1_0_0: final = {sign,exp,frac}; // normal / subnormal
      7'b0_1_0_0_0_0_x: final = {sign,exp,frac}; // subnormal / normal
      7'b0_0_0_x_0_0_x: final = {sign,exp,frac}; // normal / normal

      default: final = {sign,8'hff,qnan_frac};
    endcase
  endfunction

endmodule
