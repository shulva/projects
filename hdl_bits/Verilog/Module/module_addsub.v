
module top_module(
    input [31:0] a,
    input [31:0] b,
    input sub,
    output [31:0] sum
);

    wire [31:0] xor1 ;
    wire cout;
    assign xor1 = b^{32{sub}};

    add16 a1(a[15:0],xor1[15:0],sub,sum[15:0],cout);
    add16 a2(a[31:16],xor1[31:16],cout,sum[31:16],1'b0);
endmodule
