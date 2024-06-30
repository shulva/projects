`timescale 1ns / 1ps

module circular_queue
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
    output reg                      empty           
);

    //----------------------------------
    // Local Parameter Declarations
    //----------------------------------
    localparam FIFO_DEPTH_MINUS1    = (FIFO_DEPTH>>2)-1  ;

    //----------------------------------
    // Variable Declarations
    //----------------------------------
    reg [FIFO_WIDTH-1:0] fifo [FIFO_DEPTH>>2] ;

    reg [FIFO_PTR-1:0]              head          ;//头指针 4B为单位

    reg [FIFO_PTR-1:0]              tail          ;//尾指针 4B为单位

    reg [8:0] count [32] ;

    reg [4:0] count_head;
    reg [8:0] index_head;

    reg [4:0] count_tail;


    //--------------------------------------------------------------------------
    // head-pointer control logic
    //--------------------------------------------------------------------------
    always @(posedge clk or negedge rst_n)
    begin 

        if(!rst_n)begin
            head <= 'b0;
        end
        else if (read_en_fifo == 1'b1)begin
            read_data <= fifo[head]
            head <= head + 1'b1;
            index_head <= index_head + 1'b1;
        end
        else if(count[count_head])
            count_head <= count_head + 1'b1;
            index_head <= 9'b0;
        end

    end 
    //--------------------------------------------------------------------------
    // tail-pointer control logic
    //--------------------------------------------------------------------------
    always @(posedge clk or negedge rst_n)
    begin 

        if(!rst_n)begin
            tail <= 'b0;
        end
        else if (wr_vld == 1'b1)begin
            fifo[tail] <= write_data;
            tail <= tail + 1'b1;
            count[count_tail] <= count[count_tail] + 1'b1;
        end
        else if((wr_eop == 1'b1) && (wr_vld == 1'b0))
            count_tail <= count_tail + 1'b1;
        end
    end 

    assign full        = (tail + 1'b1 == head);
    assign empty        = (head == tail);

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



