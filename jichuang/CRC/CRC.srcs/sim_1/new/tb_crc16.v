`timescale 1ns / 1ps

module tb_crc16();

reg           clk;
reg           rst_n;
reg   [31:0]  data_in;
reg           wr_vld;
reg           wr_eop;
wire  [15:0]  crc_out;
wire          crc_finish;

initial
    begin
        clk = 1'b1;
        rst_n <= 1'b0;
        wr_vld <= 1'b0;
        wr_eop <= 1'b0;
        #20
        rst_n <= 1'b1;
        #40
        wr_vld <= 1'b1;
        #160
        wr_eop <= 1'b1;
        wr_vld <= 1'b0;
        #20
        wr_eop <= 1'b0;
    end

always #10 clk = ~clk;

initial 
    begin
    data_in <= 32'b0;
    #60
    data_in <= 32'b00000000001111111111101110011111;
    #20
    data_in <= 32'b01010101010101010101010101010111; 
    #20
    data_in <= 32'b01111111111111100000000000000000;
    #20
    data_in <= 32'b00000000001111111111111010101010;
    #20
    data_in <= 32'b00000011111111110101010001010101; 
    #20
    data_in <= 32'b01010101010000000011111111100000;
    #20
    data_in <= 32'b00000000001111111111101110011111;
    #20
    data_in <= 32'b01111111111111100000000000000000;
    #20
    data_in <= 32'b0;
    end
    
crc16 crc16_inst
(
    .clk       (clk),
    .rst_n     (rst_n),
    .data_in   (data_in), 
    .wr_vld    (wr_vld),
    .wr_eop    (wr_eop),
    .crc_out   (crc_out),
    .crc_finish(crc_finish)
);


endmodule
