
module top_module ( input clk, input d, output q );

    wire [2:0] quit;

    my_dff m1(.d(d),.clk(clk),.q(quit[0]));
    my_dff m2(.d(quit[0]),.clk(clk),.q(quit[1]));
    my_dff m3(.d(quit[1]),.clk(clk),.q(quit[2]));

    assign q = quit[2];

endmodule

/*:better
	wire a, b;// Create two wires. I called them a and b.

	// Create three instances of my_dff, with three different instance names (d1, d2, and d3).
	// Connect ports by position: ( input clk, input d, output q)
	my_dff d1 ( clk, d, a );
	my_dff d2 ( clk, a, b );
	my_dff d3 ( clk, b, q );
*/
