`timescale 1ns / 1ps

module tb_crc_top();


reg  	     clk            ;
reg  	     rst_n          ;

reg  [31:0]  write_data     ;
reg          write_en_fifo  ;
reg          read_en_fifo   ;

reg          wr_sop         ;
reg          wr_eop         ;
reg          wr_vld         ;
reg          write_arbition ;
reg  [7:0]   memory_pick    ;
reg  [7:0]   address_ready  ;
                            
wire [31:0]	 data_true      ;
wire		 write_ready    ;
wire [10:0]  data_size      ;
wire [3:0]   dest_port      ;
wire [9:0]   data_wait_time ;
wire [2:0]   data_priority  ;
wire         write_en_reg   ;


always #10 clk = ~clk;

initial
    begin
        clk = 1'b1;
        rst_n <= 1'b0;
        wr_sop <= 1'b0;
        wr_eop <= 1'b0;
        wr_vld <= 1'b0;
        write_en_fifo <= 1'b0;
        read_en_fifo <= 1'b0;
        #20
        rst_n <= 1'b1;
        #60
        write_en_fifo <= 1'b1;
        #20
        read_en_fifo <= 1'b1;
        #180
        write_en_fifo <= 1'b0;
        #20
        read_en_fifo <= 1'b0;     
    end 
    
initial
    begin
        wr_sop <= 1'b0;
        wr_eop <= 1'b0;
        wr_vld <= 1'b0;
        #20
        rst_n <= 1'b1;
        #40
        wr_sop <= 1'b1;
        #20
        wr_sop <= 1'b0;
        wr_vld <= 1'b1;
        #180
        wr_vld <= 1'b0;
        wr_eop <= 1'b1;
        #20
        wr_eop <= 1'b0;        
    end     
    

initial 
    begin
    write_data <= 32'b0;
    #60
    write_data <= 32'b000000000_00110000_01001110_101_0010;
    #20
    write_data <= 32'b00000000001111111111101110011111;
    #20
    write_data <= 32'b01010101010101010101010101010111; 
    #20
    write_data <= 32'b01111111111111100000000000000000;
    #20
    write_data <= 32'b00000000001111111111111010101010;
    #20
    write_data <= 32'b00000011111111110101010001010101; 
    #20
    write_data <= 32'b01010101010000000011111111100000;
    #20
    write_data <= 32'b00000000001111111111101110011111;
    #20
    write_data <= 32'b01111111111111100000000000000000;
    #20
    write_data <= 32'b0;
    end
    
initial
    begin
        write_arbition <= 1'b0;
        memory_pick   <= 8'b00000001;
        address_ready <= 8'b00000000;
        #700
        write_arbition <= 1'b1;
        memory_pick   <= 8'b00000010;
        address_ready <= 8'b00000001;
        #20
        memory_pick   <= 8'b00000100;
        address_ready <= 8'b00000011;
        write_arbition <= 1'b0;
        #20
        memory_pick   <= 8'b00001000;
        address_ready <= 8'b00000111;
        #20
        memory_pick   <= 8'b00010000;
        address_ready <= 8'b00011111;
    end   

CRC_top CRC_top_inst
(
    .clk           (clk           ),
	.rst_n         (rst_n         ),
                    
	.write_data    (write_data    ),
    .write_en_fifo (write_en_fifo ),
    .read_en_fifo  (read_en_fifo  ),
                    
    .wr_sop        (wr_sop        ),
    .wr_eop        (wr_eop        ),
    .wr_vld        (wr_vld        ),
    .write_arbition(write_arbition),
    .memory_pick   (memory_pick   ),  //RAM选择信号
    .address_ready (address_ready ),  //RAM就绪信号
                    
	.data_true     (data_true     ),  //crc校验结束输出的数�?
	.write_ready   (write_ready   ),  //crc正确后请求输出数据的信号�?(history_memory_pick & address_ready)>0后拉�?
    .data_size     (data_size     ),
    .dest_port     (dest_port     ),
    .data_wait_time(data_wait_time),
    .data_priority (data_priority ),
    .write_en_reg  (write_en_reg  )  
);





































endmodule
