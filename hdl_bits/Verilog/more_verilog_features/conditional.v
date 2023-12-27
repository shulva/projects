
module top_module (
    input [7:0] a, b, c, d,
    output [7:0] min);//

  wire [7:0] e;
  wire [7:0] f;
  wire [7:0] g;

    // assign intermediate_result1 = compare? true: false;
  assign e = (a<b) ? a:b;
  assign f = (c<d) ? c:d;
  assign g = (e<f) ? e:f;
  assign min = g;

endmodule
