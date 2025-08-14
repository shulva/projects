module half_adder (
    input  a,
    input  b,
    output s0,
    output c0
);
  assign s0 = a ^ b;
  assign c0 = a & b;
endmodule

module full_adder (
    input  a,
    input  b,
    input  cin,
    output s0,
    output c0
);
  assign s0 = a ^ b ^ cin;
  assign c0 = (a & b) | (b & cin) | (a & cin);
endmodule

module cla_adder (
  input [3:0] p, // p=a|b
  input [3:0] g, // g=a&b
  input cin,
  output P,
  output G,
  output [2:0] cout
);

  assign P=&p;
  assign G=g[3]|(p[3]&g[2])|(p[3]&p[2]&g[1])|(p[3]&p[2]&p[1]&g[0]);

  assign cout[0]=g[0]|(p[0]&cin);
  assign cout[1]=g[1]|(p[1]&g[0])|(p[1]&p[0]&cin);
  assign cout[2]=g[2]|(p[2]&g[1])|(p[2]&p[1]&g[0])|(p[2]&p[1]&p[0]&cin);

endmodule

module cla_48_adder (
  input wire [47:0] a,
  input wire [47:0] b,
  input wire cin,
  output wire [47:0] out,
  output carry
);

  wire [47:0] p1 = a|b;
  wire [47:0] g1 = a&b;
  wire [47:0] c;

  wire [11:0] p2;
  wire [11:0] g2;

  wire [2:0] p3;
  wire [2:0] g3;

  assign c[0] = cin;

  cla_adder cla_1(p1[3:0],g1[3:0],c[0],p2[0],g2[0],c[3:1]);
  cla_adder cla_2(p1[7:4],g1[7:4],c[4],p2[1],g2[1],c[7:5]);
  cla_adder cla_3(p1[11:8],g1[11:8],c[8],p2[2],g2[2],c[11:9]);
  cla_adder cla_4(p1[15:12],g1[15:12],c[12],p2[3],g2[3],c[15:13]);

  cla_adder cla_2_1(p2[3:0],g2[3:0],c[0],p3[0],g3[0],{c[12],c[8],c[4]});

  assign c[16] = g3[0]|(p3[0]&c[0]);

  cla_adder cla_5(p1[19:16],g1[19:16],c[16],p2[4],g2[4],c[19:17]);
  cla_adder cla_6(p1[23:20],g1[23:20],c[20],p2[5],g2[5],c[23:21]);
  cla_adder cla_7(p1[27:24],g1[27:24],c[24],p2[6],g2[6],c[27:25]);
  cla_adder cla_8(p1[31:28],g1[31:28],c[28],p2[7],g2[7],c[31:29]);

  assign c[32] = g3[1]|(p3[1]&g3[0])|(p3[1]&p3[0]&c[16]);
  cla_adder cla_2_2(p2[7:4],g2[7:4],c[16],p3[1],g3[1],{c[28],c[24],c[20]});

  cla_adder cla_9(p1[35:32],g1[35:32],c[32],p2[8],g2[8],c[35:33]);
  cla_adder cla_10(p1[39:36],g1[39:36],c[36],p2[9],g2[9],c[39:37]);
  cla_adder cla_11(p1[43:40],g1[43:40],c[40],p2[10],g2[10],c[43:41]);
  cla_adder cla_12(p1[47:44],g1[47:44],c[44],p2[11],g2[11],c[47:45]);

  cla_adder cla_2_3(p2[11:8],g2[11:8],c[32],p3[2],g3[2],{c[44],c[40],c[36]});

  assign carry = (a[47]&b[47])| (a[47]&c[47])| (a[47]&c[47]);
  assign out = a^b^c;

endmodule
