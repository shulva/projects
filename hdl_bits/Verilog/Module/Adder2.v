
module top_module (
    input [31:0] a,
    input [31:0] b,
    output [31:0] sum
);//
/*
    wire [15:0] cout;
    add1 a1(a[0],b[0],1'b0,sum[0],cout[0]);
    add1 a2(a[1],b[1],cout[0],sum[1],cout[1]);
    add1 a3(a[2],b[2],cout[1],sum[2],cout[2]);
    add1 a4(a[3],b[3],cout[2],sum[3],cout[3]);
    add1 a5(a[4],b[4],cout[3],sum[4],cout[4]);
    add1 a6(a[5],b[5],cout[4],sum[5],cout[5]);
    add1 a7(a[6],b[6],cout[5],sum[6],cout[6]);
    add1 a8(a[7],b[7],cout[6],sum[7],cout[7]);
    add1 a9(a[8],b[8],cout[7],sum[8],cout[8]);
    add1 a10(a[9],b[9],cout[8],sum[9],cout[9]);
    add1 a11(a[10],b[10],cout[9],sum[10],cout[10]);
    add1 a12(a[11],b[11],cout[10],sum[11],cout[11]);
    add1 a13(a[12],b[12],cout[11],sum[12],cout[12]);
    add1 a14(a[13],b[13],cout[12],sum[13],cout[13]);
    add1 a15(a[14],b[14],cout[13],sum[14],cout[14]);
    add1 a16(a[15],b[15],cout[14],sum[15],cout[15]);
*/
    wire d;
    add16 a1(a[15:0],b[15:0],1'b0,sum[15:0],d);
    add16 a2(a[31:16],b[31:16],d,sum[31:16],1'b0);

endmodule

module add1 ( input a, input b, input cin,   output sum, output cout );

// Full adder module here
    assign sum = a^b^cin;
    assign cout = (a&b)|(cin&b)|(cin&a);

endmodule
