module booth_24x24
(
  input  wire        imulta_sign, // 0-multa is unsigned, 1-multa is signed
  input  wire        imultb_sign, // 0-multb is unsigned, 1-multb is signed
  input  wire [23:0] imulta   , // Multiplicand
  input  wire [23:0] imultb   , // Multipler

  output wire [25:0] partial_product1     , // partial products
  output wire [25:0] partial_product2     ,
  output wire [25:0] partial_product3     ,
  output wire [25:0] partial_product4     ,
  output wire [25:0] partial_product5     ,
  output wire [25:0] partial_product6     ,
  output wire [25:0] partial_product7     ,
  output wire [25:0] partial_product8     ,
  output wire [25:0] partial_product9     ,
  output wire [25:0] partial_product10     ,
  output wire [25:0] partial_product11     ,
  output wire [25:0] partial_product12     ,
  output wire [25:0] partial_product13
);

// sign bit extend, for unsigned operator extended by 0, for signed operator extended by orignal sign bit
wire [1:0] sig_exta = ~imulta_sign ? 2'b00 : {2{imulta[23]}};
wire [1:0] sig_extb = ~imultb_sign ? 2'b00 : {2{imultb[23]}};

// generate -x, -2x, 2x for Booth encoding
wire [25:0] x     =  {sig_exta, imulta};
wire [25:0] x_c   = ~x + 1;    // -x, complement code of x
wire [25:0] x2   =  x << 1;   // 2*x,位数正好
wire [25:0] x_c2 =  x_c << 1; // -2*x

//        26 25          [24        ...                     2 1]     0  
//        |---|          |------------------------------------|      |
// extended sign bits              orignal operator             appended bit for encoding

wire [26:0] y     =  {sig_extb, imultb, 1'b0};

// calculating partial product based on Booth Radix-4 encoding
wire [25:0] pp[12:0];

genvar i;
generate
  for(i=0; i<26; i=i+2) begin : GEN_PP
    assign pp[i/2] = (y[i+2:i] == 3'b001 || y[i+2:i] == 3'b010) ? x     :
                     (y[i+2:i] == 3'b101 || y[i+2:i] == 3'b110) ? x_c   :
        	     (y[i+2:i] == 3'b011                      ) ? x2   :
        	     (y[i+2:i] == 3'b100                      ) ? x_c2 : 26'b0;
  end
endgenerate



assign partial_product1 = pp[0];
assign partial_product2 = pp[1];
assign partial_product3 = pp[2];
assign partial_product4 = pp[3];
assign partial_product5 = pp[4];
assign partial_product6 = pp[5];
assign partial_product7 = pp[6];
assign partial_product8 = pp[7];
assign partial_product9 = pp[8];
assign partial_product10 = pp[9];
assign partial_product11 = pp[10];
assign partial_product12 = pp[11];
assign partial_product13 = pp[12];

endmodule
/*
             *
    case (y[i+2:i])
       3'b001: assign pp[i/2] = x;
       3'b010: assign pp[i/2] = x;

       3'b101: assign pp[i/2] = x_c;
       3'b110: assign pp[i/2] = x_c;

       3'b011: assign pp[i/2] = x2;

       3'b100: assign pp[i/2] = x_c2;

       3'b001: assign pp[i/2] = 26'b0;
       3'b001: assign pp[i/2] = 26'b0;
    endcase
*/
