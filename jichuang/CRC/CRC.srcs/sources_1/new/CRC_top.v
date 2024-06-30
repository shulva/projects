`timescale 1ns / 1ps

module CRC_top
(
    input				 clk           ,
    input				 rst_n         ,
    input   [31:0]       write_data    ,
    input                write_en_fifo ,
    input                read_en_fifo  ,
    
    input                wr_sop        ,
    input                wr_eop        ,
    input                wr_vld        ,
    input                write_arbition,
    input   [7:0]        memory_pick   ,  //RAM选择信号
    input   [7:0]        address_ready ,  //RAM就绪信号
                                       
    output	wire [31:0]	 data_true     ,  //crc校验结束输出的数据
    output	wire		 write_ready   ,  //crc正确后请求输出数据的信号，(history_memory_pick & address_ready)>0后拉低
    output  wire [10:0]  data_size     ,
    output  wire [3:0]   dest_port     ,
    output  wire [9:0]   data_wait_time,
    output  wire [2:0]   data_priority ,
    output  wire         write_en_reg

);
    
wire  [31:0]         read_data ;
wire                 full      ;
wire                 empty     ;
wire  [10:0]         room_avail;
wire  [10:0]         data_avail;
    

 synch_fifo
#(
    //----------------------------------
    // Parameter Declarations
    //----------------------------------
    .FIFO_PTR  (9     )       ,  //指针
    .FIFO_WIDTH(32     )       ,
    .FIFO_DEPTH(2048   )     
)
synch_fifo_inst
(
    //----------------------------------
    // IO Declarations
    //----------------------------------
    .clk             (clk),
    .rst_n           (rst_n),
    .write_en_fifo   (write_en_fifo),
    .write_data      (write_data ),
    .read_en_fifo    (read_en_fifo),
    .read_data       (read_data  ),
    .full            (full       ),
    .empty           (empty      ),
    .room_avail      (room_avail ),
    .data_avail      (data_avail )
);   
    
    
crc_new crc_new_inst
(
	.clk           (clk           ),
	.rst_n         (rst_n         ),
	.data_in       (read_data),
    .wr_sop        (wr_sop        ),
    .wr_eop        (wr_eop        ),
    .wr_vld        (wr_vld        ),
    .write_arbition(write_arbition),
    .memory_pick   (memory_pick   ),  //RAM选择信号
    .address_ready (address_ready ),  //RAM就绪信号
                               
	.data_true     (data_true     ),  //crc校验结束输出的数据
	.write_ready   (write_ready   ),  //crc正确后请求输出数据的信号，(history_memory_pick & address_ready)>0后拉低
    .data_size     (data_size     ),
    .dest_port     (dest_port     ),
    .data_wait_time(data_wait_time),
    .data_priority (data_priority ),
    .write_en_reg  (write_en_reg  )   //输出同步使能
);   
    
       
    
    
endmodule
