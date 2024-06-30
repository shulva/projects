`timescale 1ns / 1ps

module synch_fifo
#(
    //----------------------------------
    // Parameter Declarations
    //----------------------------------
    parameter FIFO_PTR              = 9            ,  //指针
    parameter FIFO_WIDTH            = 32           ,
    parameter FIFO_DEPTH            = 2048
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

    input                wr_sop        ,
    input                wr_eop        ,
    input                wr_vld        ,

    output [FIFO_WIDTH-1:0]         read_data       ,
    output reg                      full            ,
    output reg                      empty           ,
    output reg [FIFO_PTR+1:0]         room_avail      ,
    output [FIFO_PTR+1:0]             data_avail
);

    //----------------------------------
    // Local Parameter Declarations
    //----------------------------------
    localparam FIFO_DEPTH_MINUS1    = (FIFO_DEPTH>>2)-1  ;

    //----------------------------------
    // Variable Declarations
    //----------------------------------
    reg [FIFO_WIDTH-1:0] fifo [FIFO_DEPTH>>2] ;

    reg [FIFO_PTR-1:0]              head          ;//头指针
    reg [FIFO_PTR-1:0]              head_nxt      ;

    reg [FIFO_PTR-1:0]              tail          ;//尾指针
    reg [FIFO_PTR-1:0]              tail_nxt      ;

    reg [7:0] count [32] ;


    wire                            full_nxt        ;
    wire                            empty_nxt       ;
    wire [FIFO_PTR:0]               room_avail_nxt  ;


    //--------------------------------------------------------------------------
    // head-pointer control logic
    //--------------------------------------------------------------------------
    always @(*)
    begin 
        head_nxt = head ;
        
        if (write_en_fifo) begin
            if (wr_ptr == FIFO_DEPTH_MINUS1)
                wr_ptr_nxt = 'd0;
            else
                wr_ptr_nxt = wr_ptr + 1'b1;
        end
    end 

    //--------------------------------------------------------------------------
    // tail-pointer control logic
    //--------------------------------------------------------------------------
    always @(posedge clk or negedge rst_n)
    begin 
        if(!rst_n)begin
            tail <= 'b0;
            tail_nxt <= 'b0;
        end
        else if (wr_vld == 1'b1)begin
          fifo[tail] <= write_data;
          tail <= tail + 1;

        end
        
        rd_ptr_nxt = rd_ptr;
        

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
            head      <= 9'b0;
            tail      <= 9'b0;
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



