//------------------------------------------------------------------------------
// File					: synch_fifo
// Author				: FFQ
// Key Words				:
// Modification History		        :
// 	Date    	By      	Version    	Change Description
//	2021-03-19	FFQ		1.0		original
//
// Editor				: VSCode, Tab Size (4)
// Description				:
//
// (C) Copyright 2021 Institute of Digital Economy Industry. 
// All rights reserved.
//------------------------------------------------------------------------------
`timescale 1ns / 1ps

module synch_fifo
#(
    //----------------------------------
    // Parameter Declarations
    //----------------------------------
    parameter FIFO_PTR              = 10            ,  //指针
    parameter FIFO_WIDTH            = 32            ,
    parameter FIFO_DEPTH            = 1024        
)
(
    //----------------------------------
    // IO Declarations
    //----------------------------------
    input                           clk             ,
    input                           rst_n           ,
    input                           write_en_fifo   ,
    input [FIFO_WIDTH-1:0]          write_data      ,
    input                           read_en_fifo    ,
    output [FIFO_WIDTH-1:0]         read_data       ,
    output reg                      full            ,
    output reg                      empty           ,
    output reg [FIFO_PTR:0]         room_avail      ,
    output [FIFO_PTR:0]             data_avail  
);

    //----------------------------------
    // Local Parameter Declarations
    //----------------------------------
    localparam FIFO_DEPTH_MINUS1    = FIFO_DEPTH-1  ;

    //----------------------------------
    // Variable Declarations
    //----------------------------------
    reg [FIFO_PTR-1:0]              wr_ptr          ;//写指针
    reg [FIFO_PTR-1:0]              wr_ptr_nxt      ;

    reg [FIFO_PTR-1:0]              rd_ptr          ;//读指针
    reg [FIFO_PTR-1:0]              rd_ptr_nxt      ;

    reg [FIFO_PTR:0]                num_entries     ;
    reg [FIFO_PTR:0]                num_entries_nxt ;

    wire                            full_nxt        ;
    wire                            empty_nxt       ;

    wire [FIFO_PTR:0]               room_avail_nxt  ;

    //--------------------------------------------------------------------------
    // write-pointer control logic
    //--------------------------------------------------------------------------
    always @(*)
    begin 
        wr_ptr_nxt = wr_ptr;
        
        if (write_en_fifo) begin
            if (wr_ptr == FIFO_DEPTH_MINUS1)
                wr_ptr_nxt = 'd0;
            else
                wr_ptr_nxt = wr_ptr + 1'b1;
        end
    end 

    //--------------------------------------------------------------------------
    // read-pointer control logic
    //--------------------------------------------------------------------------
    always @(*)
    begin 
        rd_ptr_nxt = rd_ptr;
        
        if (read_en_fifo) begin
            if (rd_ptr == FIFO_DEPTH_MINUS1)
                rd_ptr_nxt = 'd0;
            else
                rd_ptr_nxt = rd_ptr + 1'b1;
        end
    end 

    //--------------------------------------------------------------------------
    // calculate number of occupied entries in the FIFO
    //--------------------------------------------------------------------------
    always @(*)
    begin
        num_entries_nxt = num_entries;

        if (write_en_fifo && read_en_fifo)
            num_entries_nxt = num_entries;
        else if (write_en_fifo)
            num_entries_nxt = num_entries + 1'b1;
        else if (read_en_fifo)
            num_entries_nxt = num_entries - 1'b1;
    end

    assign full_nxt         = (num_entries_nxt == FIFO_DEPTH);
    assign empty_nxt        = (num_entries_nxt == 'd0);
    assign data_avail       = num_entries;
    assign room_avail_nxt   = (FIFO_DEPTH - num_entries_nxt);

    //--------------------------------------------------------------------------
    // register output
    //--------------------------------------------------------------------------
    always @(posedge clk or negedge rst_n)
    begin
        if (!rst_n) begin
            wr_ptr      <= 'd0;
            rd_ptr      <= 'd0;
            num_entries <= 'd0;
            full        <= 1'b0;
            empty       <= 1'b1;
            room_avail  <= FIFO_DEPTH;
        end
        else begin
            wr_ptr      <= wr_ptr_nxt;
            rd_ptr      <= rd_ptr_nxt;
            num_entries <= num_entries_nxt;
            full        <= full_nxt;
            empty       <= empty_nxt;
            room_avail  <= room_avail_nxt;
        end
    end 

    //--------------------------------------------------------------------------
    // SRAM memory instantiation
    //--------------------------------------------------------------------------
/*     sram
    #(
	.PTR			(FIFO_PTR		),
	.FIFO_WIDTH		(FIFO_WIDTH		)
    )
    u_sram
    (
	.wrclk			(clk			),
	.wren			(write_en_fifo	),
	.wrptr			(wr_ptr			),
	.wrdata			(write_data		),
	.rdclk			(clk			),
	.rden			(read_en_fifo	),
	.rdptr			(rd_ptr			),
	.rddata			(read_data		)
    ); */

endmodule



