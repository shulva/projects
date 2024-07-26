`timescale 1ns / 1ps


module crossbar
(
    //----------------------------------
    // IO Declarations
    //----------------------------------
    input                clk             ,
    input                rst_n           ,

    input                wr_sop_1        ,
    input                wr_eop_1        ,
    input                wr_vld_1        ,
    input                room_avail_1    ,
    input                full_1          ,
    input                empty_1         ,

    input                wr_sop_2        ,
    input                wr_eop_2        ,
    input                wr_vld_2        ,
    input                room_avail_2    ,
    input                full_2          ,
    input                empty_2         ,
    

    input                wr_sop_3        ,
    input                wr_eop_3        ,
    input                wr_vld_3        ,
    input                room_avail_3    ,
    input                full_3          ,
    input                empty_3         ,

    input                wr_sop_4        ,
    input                wr_eop_4        ,
    input                wr_vld_4        ,
    input                room_avail_4    ,
    input                full_4          ,
    input                empty_4         ,


    input [FIFO_WIDTH-1:0]          write_data_1      ,
    input [FIFO_WIDTH-1:0]          write_data_2      ,
    input [FIFO_WIDTH-1:0]          write_data_3      ,
    input [FIFO_WIDTH-1:0]          write_data_4      ,

    output [FIFO_WIDTH-1:0] read_data_1,
    output [FIFO_WIDTH-1:0] read_data_2,
    output [FIFO_WIDTH-1:0] read_data_3,
    output [FIFO_WIDTH-1:0] read_data_4
 
);

circular_queue
#(
    //----------------------------------
    // Parameter Declarations
    //----------------------------------
    .FIFO_PTR  (9     )       ,  
    .FIFO_WIDTH(32     )       ,
    .FIFO_DEPTH(2048   )     
)
cq1
(
    //----------------------------------
    // IO Declarations
    //----------------------------------
    .clk             (clk),
    .rst_n           (rst_n),
    .write_data      (write_data_1 ),
    .read_en_fifo    (read_en_1),
    .wr_sop        (wr_sop_1        ),
    .wr_eop        (wr_eop_1        ),
    .wr_vld        (wr_vld_1        ),

    .read_data       (read_data_1  ),
    .full            (full_1       ),
    .empty           (empty_1      ),
    .room_avail      (room_avail_1)
);   

circular_queue
#(
    //----------------------------------
    // Parameter Declarations
    //----------------------------------
    .FIFO_PTR  (9     )       ,  
    .FIFO_WIDTH(32     )       ,
    .FIFO_DEPTH(2048   )     
)
cq2
(
    //----------------------------------
    // IO Declarations
    //----------------------------------
    .clk             (clk),
    .rst_n           (rst_n),
    .write_data      (write_data_2 ),
    .read_en_fifo    (read_en_1),
    .wr_sop        (wr_sop_2        ),
    .wr_eop        (wr_eop_2        ),
    .wr_vld        (wr_vld_2        ),

    .read_data       (read_data_2  ),
    .full            (full_2       ),
    .empty           (empty_2      ),
    .room_avail      (room_avail_2)
);   

circular_queue
#(
    //----------------------------------
    // Parameter Declarations
    //----------------------------------
    .FIFO_PTR  (9     )       ,  
    .FIFO_WIDTH(32     )       ,
    .FIFO_DEPTH(2048   )     
)
cq3
(
    //----------------------------------
    // IO Declarations
    //----------------------------------
    .clk             (clk),
    .rst_n           (rst_n),
    .write_data      (write_data_3 ),
    .read_en_fifo    (read_en_1),
    .wr_sop        (wr_sop_3        ),
    .wr_eop        (wr_eop_3        ),
    .wr_vld        (wr_vld_3        ),

    .read_data       (read_data_3  ),
    .full            (full_3       ),
    .empty           (empty_3      ),
    .room_avail      (room_avail_3)
);   


circular_queue
#(
    //----------------------------------
    // Parameter Declarations
    //----------------------------------
    .FIFO_PTR  (9     )       ,  
    .FIFO_WIDTH(32     )       ,
    .FIFO_DEPTH(2048   )     
)
cq4
(
    //----------------------------------
    // IO Declarations
    //----------------------------------
    .clk             (clk),
    .rst_n           (rst_n),
    .write_data      (write_data_4 ),
    .read_en_fifo    (read_en_1),
    .wr_sop        (wr_sop_4        ),
    .wr_eop        (wr_eop_4        ),
    .wr_vld        (wr_vld_4        ),

    .read_data       (read_data_4  ),
    .full            (full_4       ),
    .empty           (empty_4      ),
    .room_avail      (room_avail_4)
);   

  reg [2:0] idle;

  always @(posedge clk)
    begin
      if (wr_sop_1 == 1'b0 || wr_vld_1 == 1'b0 && full_1 == 1'b0)
        idle = 3'b001;
      else if (wr_sop_2 == 1'b0 || wr_vld_2 == 1'b0 && full_2 == 1'b0)
        idle = 3'b010;
      else if (wr_sop_3 == 1'b0 || wr_vld_3 == 1'b0 && full_3 == 1'b0)
        idle = 3'b011;
      else if (wr_sop_4 == 1'b0 || wr_vld_4 == 1'b0 && full_4 == 1'b0)
        idle = 3'b100;
      else
        idle = 3'b000; //无空闲
    end


endmodule




