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
    input [FIFO_WIDTH-1:0]          write_data      ,
    input                           read_en_fifo    ,
    //input crc_finish,
    //input crc_true,

    input                wr_sop        ,
    input                wr_eop        ,
    input                wr_vld        ,

    output reg [FIFO_WIDTH-1:0]         read_data       ,
    output                       full            ,
    output                       empty,
    output reg [FIFO_PTR-1:0] room_avail  //以B为单位
);

    //----------------------------------
    // Local Parameter Declarations
    //----------------------------------
    localparam FIFO_DEPTH_MINUS1    = (FIFO_DEPTH>>2)-1  ;

    //----------------------------------
    // Variable Declarations
    //----------------------------------
    reg [FIFO_WIDTH-1:0] fifo [FIFO_DEPTH_MINUS1:0] ;

    reg [FIFO_PTR-1:0]              head          ;//头指针以 4B为单位

    reg [FIFO_PTR-1:0]              tail          ;//尾指针以 4B为单位
    reg [FIFO_PTR-1:0]              history_tail          ;

    reg [8:0] count [31:0] ; //包长度数组

    reg [4:0] count_head;
    reg [8:0] index_head;

    reg [4:0] count_tail;


    //--------------------------------------------------------------------------
    // head-pointer control logic
    //--------------------------------------------------------------------------
      always @(posedge clk or negedge rst_n)
      begin 

        if(!rst_n)
          begin
            head <= 9'b0;
          end
        else if (read_en_fifo == 1'b1 && (empty != 1'b1))
          begin
            read_data <= fifo[head];
            head <= head + 1'b1;
            index_head <= index_head + 1'b1;
          end
        else if(count[count_head])
          begin
            count_head <= count_head + 1'b1;
            index_head <= 9'b0;
          end
      end
    //--------------------------------------------------------------------------
    // tail-pointer control logic
    //--------------------------------------------------------------------------
    always @(posedge clk or negedge rst_n)
    begin 
      if(!rst_n)
        begin
          tail <= 9'b0;
        end
      else if (wr_sop == 1'b1)
        begin
          history_tail <= tail;
        end
      else if (wr_vld == 1'b1)
        begin
          fifo[tail] <= write_data;
          tail <= tail + 1'b1;
          count[count_tail] <= count[count_tail] + 1'b1;
        end
      else if((wr_eop == 1'b1) && (wr_vld == 1'b0))
        begin
          count_tail <= count_tail + 1'b1;
        end
    end

    assign full        = (tail + 1'b1 == head);
    assign empty        = (head == tail);

    //--------------------------------------------------------------------------
    // register output
    //--------------------------------------------------------------------------
    integer i,j;
    always @(posedge clk or negedge rst_n)
    begin
        if (!rst_n) begin
            head      <= 9'b0;
            tail      <= 9'b0;
            history_tail<= 9'b0;
            count_head <= 5'b0;
            count_tail <= 5'b0;
            index_head<= 9'b0;
            for(j=0; j<512; j=j+1)
                fifo[j] <= 32'd0;

            for(i=0; i<32; i=i+1)
                count[i] <= 9'd0;
        end
    end 

    //--------------------------------------------------------------------------
    // check crc
    //--------------------------------------------------------------------------
    /*
    reg crc_finished;
    always @(posedge clk)
    begin
      if (crc_finish == 1'b1)
      crc_finished <= 1'b1;

      if(crc_finished == 1'b1)           
      begin
        if (crc_true == 1'b0)
          tail <= history_tail;

        crc_finished <= 1'b0;
      end
    end
    */

    //--------------------------------------------------------------------------
    // room avail calculate
    //--------------------------------------------------------------------------
    always @(posedge clk)
    begin
      if (head > tail)
          room_avail <= (head - tail)<<2;
      else if (head < tail)
          room_avail <= (512 - tail + head)<<2;
      else if (head == tail)
          room_avail <= 9'b0;
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



