`timescale 1ns / 1ps

module sram
#(
    parameter PTR 			    = 10		    ,
    parameter FIFO_WIDTH		= 32			,
    parameter A_MAX 			= 2**(PTR)
)
(
    
    input                		    wrclk	,
    input [PTR-1:0] 			    wrptr	,
    input [FIFO_WIDTH-1:0] 		    wrdata	,
    input                		    wren	,
                                                    
    input           			    rdclk   ,
    input [PTR-1:0] 			    rdptr	,
    input                		    rden	,
    output reg [FIFO_WIDTH-1:0]     rddata			
);

    reg [FIFO_WIDTH-1:0]   memory[A_MAX-1:0];
	
    // Write data to memory
    always @(posedge wrclk) 
    begin
        if (wren) begin
	    memory[wrptr] <= wrdata;	
	end
    end

    // Read data from memory
    always @(posedge rdclk) 
    begin
	if (rden) begin
	    rddata <= memory[rdptr];
	end
    end
	
endmodule
