module shift(
  input [23:0] a, // shift a=xxxx... to b=1xxxx....
  output [23:0] b,// 1xxxxx
  output [4:0] move_num
);

  wire [23:0] r5,r4,r3,r2,r1,r0;

  assign r5 = a;

  assign move_num[4] = ~|r5[23:8];
  assign r4 = move_num[4]?{r5[7:0],16'b0}:r5; //16个0

  assign move_num[3] = ~|r4[23:16];
  assign r3 = move_num[3]?{r4[15:0],8'b0}:r4; //8个0

  assign move_num[2] = ~|r3[23:20];
  assign r2 = move_num[2]?{r3[19:0],4'b0}:r3; //4个0

  assign move_num[1] = ~|r2[23:22];
  assign r1 = move_num[1]?{r2[21:0],2'b0}:r2; //2个0

  assign move_num[0] = ~r1[23];
  assign r0 = move_num[0]?{r1[22:0],1'b0}:r1; //1个0

  assign b = r0;

endmodule
