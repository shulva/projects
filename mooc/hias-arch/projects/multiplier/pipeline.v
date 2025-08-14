module pipeline_float_mul(
  input [31:0] input_a,
  input [31:0] input_b,
  output sign,
  output [9:0] exp,
  output s_is_nan,
  output s_is_inf,
  output [22:0] qnan_frac,
  output [47:0] z_sum,
  output [47:0] z_carry
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

  wire a_is_0 =a_expo_is_00&a_frac_is_00;
  wire b_is_0 =b_expo_is_00&b_frac_is_00;


  wire [23:0] frac_a = {~a_expo_is_00,input_a[22:0]};
  wire [23:0] frac_b = {~b_expo_is_00,input_b[22:0]};

  assign sign = input_a[31]^input_b[31];
  assign qnan_frac = (a_is_nan)?{1'b1,input_a[21:0]}:{1'b1,input_b[21:0]}; //qNan
  assign exp = {2'b0,input_a[30:23]}+{2'b0,input_b[30:23]}- 127 + a_expo_is_00 +b_expo_is_00;


  assign s_is_inf=a_is_inf|b_is_inf;
  assign s_is_nan=a_is_nan|(a_is_inf&b_is_0)|b_is_nan|(b_is_inf&a_is_0);

// unpack ------------------------------------------------------

  wire [25:0] partial_product [12:0];
  wire [47:0] partial_product_extend [12:0];

  wire [9:0] z_out;

  booth_24x24 booth(1'b0,1'b0,frac_a,frac_b,
    partial_product[0],partial_product[1],partial_product[2],partial_product[3],partial_product[4],
    partial_product[5],partial_product[6],partial_product[7],partial_product[8],partial_product[9],
    partial_product[10],partial_product[11],partial_product[12]);

  assign partial_product_extend[0] = {{22{partial_product[0][25]}},partial_product[0]};
  assign partial_product_extend[1] = {{22{partial_product[1][25]}},partial_product[1]}<<2;
  assign partial_product_extend[2] = {{22{partial_product[2][25]}},partial_product[2]}<<4;
  assign partial_product_extend[3] = {{22{partial_product[3][25]}},partial_product[3]}<<6;
  assign partial_product_extend[4] = {{22{partial_product[4][25]}},partial_product[4]}<<8;
  assign partial_product_extend[5] = {{22{partial_product[5][25]}},partial_product[5]}<<10;
  assign partial_product_extend[6] = {{22{partial_product[6][25]}},partial_product[6]}<<12;
  assign partial_product_extend[7] = {{22{partial_product[7][25]}},partial_product[7]}<<14;
  assign partial_product_extend[8] = {{22{partial_product[8][25]}},partial_product[8]}<<16;
  assign partial_product_extend[9] = {{22{partial_product[9][25]}},partial_product[9]}<<18;
  assign partial_product_extend[10] = {{22{partial_product[10][25]}},partial_product[10]}<<20;
  assign partial_product_extend[11] = {{22{partial_product[11][25]}},partial_product[11]}<<22;
  assign partial_product_extend[12] = {{22{partial_product[12][25]}},partial_product[12]}<<24;
  
  wallace_tree_24 wallace(clk,rst,partial_product_extend[0],partial_product_extend[1],partial_product_extend[2],partial_product_extend[3],
    partial_product_extend[4],partial_product_extend[5],partial_product_extend[6],partial_product_extend[7],partial_product_extend[8],
    partial_product_extend[9],partial_product_extend[10],partial_product_extend[11],partial_product_extend[12],
    10'b0000000000,z_carry,z_sum,z_out);

endmodule

module reg_mul_add(
  input clrn,
  input enable,
  input clk,
  input sign,
  input [9:0] exp,
  input s_is_nan,
  input s_is_inf,
  input [22:0] qnan_frac,
  input [47:0] z_sum,
  input [47:0] z_carry,

  output reg o_sign,
  output reg [9:0] o_exp,
  output reg o_s_is_nan,
  output reg o_s_is_inf,
  output reg [22:0] o_qnan_frac,
  output reg [47:0] o_z_sum,
  output reg [47:0] o_z_carry
);

  always @(posedge clk or negedge clrn) begin
    if (clrn == 0) begin
      o_sign <= 0;
      o_exp <= 0;
      o_s_is_nan <= 0;
      o_s_is_inf <= 0;
      o_qnan_frac <= 0;
      o_z_sum <= 0;
      o_z_carry <= 0;
    end
    else if (enable) begin
      o_sign <= sign;
      o_exp <= exp;
      o_s_is_nan <= s_is_nan;
      o_s_is_inf <= s_is_inf;
      o_qnan_frac <= qnan_frac;
      o_z_sum <= z_sum;
      o_z_carry <= z_carry;
    end
  end
endmodule

module reg_add_normalize(
  input clrn,
  input enable,
  input clk,
  input sign,
  input [9:0] exp,
  input s_is_nan,
  input s_is_inf,
  input [22:0] qnan_frac,
  input [47:0] temp_sum,

  output reg o_sign,
  output reg [9:0] o_exp,
  output reg o_s_is_nan,
  output reg o_s_is_inf,
  output reg [22:0] o_qnan_frac,
  output reg [47:0] o_temp_sum
);

  always @(posedge clk or negedge clrn) begin
    if (clrn == 0) begin
      o_sign <= 0;
      o_exp <= 0;
      o_s_is_nan <= 0;
      o_s_is_inf <= 0;
      o_qnan_frac <= 0;
      o_temp_sum <= 0;
    end
    else if (enable) begin
      o_sign <= sign;
      o_exp <= exp;
      o_s_is_nan <= s_is_nan;
      o_s_is_inf <= s_is_inf;
      o_qnan_frac <= qnan_frac;
      o_temp_sum <= temp_sum;
    end
  end
endmodule

module float_add_normalize(
  input clk,
  input sign,
  input [9:0] exp,
  input s_is_nan,
  input s_is_inf,
  input [22:0] qnan_frac,
  input [47:0] temp_sum,
  output [31:0] q
);

// frac * frac ------------------------------------------------------

  wire [46:0] r5,r4,r3,r2,r1,r0; //z[47]x.xxxxxx...
  wire [5:0] move_exp;

  assign move_exp[5] = ~|temp_sum[46:15];
  assign r5 = move_exp[5]?{temp_sum[14:0],32'b0}:temp_sum[46:0]; // 32个0

  assign move_exp[4] = ~|temp_sum[46:31];
  assign r4 = move_exp[4]?{temp_sum[30:0],16'b0}:r5; //16个0

  assign move_exp[3] = ~|temp_sum[46:39];
  assign r3 = move_exp[3]?{temp_sum[38:0],8'b0}:r4; //8个0

  assign move_exp[2] = ~|temp_sum[46:43];
  assign r2 = move_exp[2]?{temp_sum[42:0],4'b0}:r3; //4个0

  assign move_exp[1] = ~|temp_sum[46:45];
  assign r1 = move_exp[1]?{temp_sum[44:0],2'b0}:r2; //2个0

  assign move_exp[0] = ~temp_sum[46];
  assign r0 = move_exp[0]?{temp_sum[45:0],1'b0}:r1; //1个0

  reg [22:0] temp_frac;
  reg [9:0] temp_exp;

  always @(*) begin
    if(temp_sum[47]) begin //1x.xxxx...
      temp_exp = exp+1;
      temp_frac = temp_sum[46:24]; //1.xxxx...
    end
    else begin
      if (!exp[9] &&(exp[8:0]>move_exp)&& r0[46]) begin
        temp_exp = exp-move_exp;
        temp_frac = r0[45:23];
      end
      else begin  // 0 or subnormal , underflow may happen
        temp_exp = 0;
        if (!exp[9] && (exp != 0 ))  begin // exp>0
          temp_frac = temp_sum[46:24] << (exp-1);
        end
        else begin
          temp_frac = temp_sum[46:24] >> (1-exp); // 可能会有精度问题
        end
      end
    end

    if(temp_sum[47] & temp_sum[23]) begin
      temp_frac = temp_frac+1;
    end
    else if (~temp_sum[47] & temp_sum[22]) begin
      temp_frac = temp_frac+1;
    end
  end
//normalize and roundTiesToAway ---------------------------------------------------------


  wire overflow = (temp_exp >10'h0ff) ;

  assign q = final(overflow,s_is_nan,s_is_inf,sign,temp_exp,temp_frac);


// end -----------------------------------------------------------------

  function [31:0] final;
    input overflow;
    input s_is_nan;
    input s_is_inf;
    input sign;
    input [7:0] exp;
    input [22:0] frac;
    
    casex ({overflow,sign,s_is_nan,s_is_inf})
      4'b1_x_0_x:final = {sign,8'hff,23'h000000}; //inf
      4'b0_x_0_0:final = {sign,exp,frac}; //ok
      4'bx_x_1_x:final = {sign,8'hff,qnan_frac};// nan
      4'bx_x_0_1:final = {sign,8'hff,23'h000000};// inf
      default: final = {sign,8'h00,23'h000000}; // signed zero
    endcase
  endfunction
endmodule
