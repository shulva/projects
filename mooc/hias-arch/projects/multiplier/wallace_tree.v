module wallace_tree(
  input clk    ,
  input rst    ,
  input [12:0] a,
  input [9:0] cin,
  output [9:0] cout,
  output s,
  output carry
);

  wire [9:0] s0;

  full_adder l1_1(a[0],a[1],a[2],s0[0],cout[0]);
  full_adder l1_2(a[3],a[4],a[5],s0[1],cout[1]);
  full_adder l1_3(a[6],a[7],a[8],s0[2],cout[2]);
  full_adder l1_4(a[9],a[10],a[11],s0[3],cout[3]);

  full_adder l2_1(s0[0],s0[1],s0[2],s0[4],cout[4]);
  full_adder l2_2(s0[3],cin[0],cin[1],s0[5],cout[5]);
  full_adder l2_3(cin[2],cin[3],a[12],s0[6],cout[6]);

  full_adder l3_1(s0[4],s0[5],s0[6],s0[7],cout[7]);
  full_adder l3_2(cin[4],cin[5],cin[6],s0[8],cout[8]);

  full_adder l4(s0[7],s0[8],cin[7],s0[9],cout[9]);

  full_adder l5(s0[9],cin[8],cin[9],s,carry);

endmodule

module wallace_tree_24(
  input clk,
  input rst,
  input [47:0] pp1,
  input [47:0] pp2,
  input [47:0] pp3,
  input [47:0] pp4,
  input [47:0] pp5,
  input [47:0] pp6,
  input [47:0] pp7,
  input [47:0] pp8,
  input [47:0] pp9,
  input [47:0] pp10,
  input [47:0] pp11,
  input [47:0] pp12,
  input [47:0] pp13,

  input [9:0] cin,
  output [47:0] carry,
  output [47:0] sum,
  output [9:0] cout
);

  wire [9:0] connect [48:0];
  assign connect[0] = cin;

  genvar i;
  generate
    for (i = 0; i < 48; i=i+1) begin :w_t
      wallace_tree w_t(clk,rst,
      {pp1[i],pp2[i],pp3[i],pp4[i],pp5[i],pp6[i],pp7[i],pp8[i],pp9[i],pp10[i],pp11[i],pp12[i],pp13[i]},
      connect[i],connect[i+1],sum[i],carry[i]);
    end
  endgenerate

  assign cout = connect[48];

endmodule
