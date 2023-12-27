module top_module ( 
    input clk, 
    input [7:0] d, 
    input [1:0] sel, 
    output [7:0] q 
);

    wire [7:0] quit [2:0];

    my_dff8 m1(.d(d),.clk(clk),.q(quit[0]));
    my_dff8 m2(.d(quit[0]),.clk(clk),.q(quit[1]));
    my_dff8 m3(.d(quit[1]),.clk(clk),.q(quit[2]));

    always @(*) begin
      case(sel)
        0:q=d;
        1:q=quit[0];
        2:q=quit[1];
        3:q=quit[2];
      endcase
    end
      
endmodule
