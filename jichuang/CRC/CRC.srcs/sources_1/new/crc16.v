`timescale 1ns / 1ps

module crc16
(
    input   wire           clk       ,
    input   wire           rst_n     ,
    input   wire  [31:0]   data_in   , 
    input   wire           wr_vld    ,
    input   wire           wr_eop    ,
    
    output  reg   [15:0]   crc_out   ,
    output  reg            crc_finish
);

reg [15:0] crc_reg ;
reg [15:0] crc_table [0:255]; 

initial $readmemh("crc_table.txt",crc_table); 

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) 
        crc_reg <= 16'hFFFF;
    else if(wr_vld == 1'b1)begin
        crc_reg <= crc_reg[15:8] ^ (crc_table[crc_reg[7:0] ^ data_in[31:24]]);
        crc_reg <= crc_reg[15:8] ^ (crc_table[crc_reg[7:0] ^ data_in[23:16]]);
        crc_reg <= crc_reg[15:8] ^ (crc_table[crc_reg[7:0] ^ data_in[15:8]]);
        crc_reg <= crc_reg[15:8] ^ (crc_table[crc_reg[7:0] ^ data_in[7:0]]); 
    end 
    else if(wr_eop)
        crc_reg <= 16'hFFFF;
end 

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) 
        crc_out <= 16'b0;
    else if(wr_eop)
        crc_out <= crc_reg ^ 16'h0000;
    else
        crc_out <= crc_out;
end

always @(posedge clk or negedge rst_n) begin
    if (!rst_n)
        crc_finish <= 1'b0;
    else if(wr_eop)
        crc_finish <= 1'b1;
    else 
        crc_finish <= 1'b0;
end 

endmodule
