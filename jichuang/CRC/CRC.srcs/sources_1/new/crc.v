module crc
(
	input				 clk           ,
	input				 rst_n         ,
	input   [31:0]       data_in       ,
    input                wr_sop        ,
    input                wr_eop        ,
    input                wr_vld        ,
    input                write_arbition,
    input   [7:0]        memory_pick   ,  //RAM选择信号
    input   [7:0]        address_ready ,  //RAM就绪信号
                                       
	output	reg  [31:0]	 data_true     ,  //crc校验结束输出的数据
	output	reg			 write_ready   ,  //crc正确后请求输出数据的信号，(history_memory_pick & address_ready)>0后拉低
    output  reg  [10:0]  data_size     ,
    output  reg  [3:0]   dest_port     ,
    output  reg  [9:0]   data_wait_time,
    output  reg  [2:0]   data_priority ,
    output  reg          write_enable     //输出同步使能
);
 
reg   [8:0]   data_size_cnt;       //最大256次
reg           wr_eop_reg;          //输入数据结束打一拍（CRC落后一拍）
reg   [8:0]   data_out_cnt = 9'd0; //输出数据计数
reg   [31:0]  data_real;           //需校验的有效数据（去包头）
reg           crc_finish;          //crc结束标志
reg			  crc_true;            //CRC校验正确
reg   [15:0]  crc_in;              //输入数据的crc值
reg   [15:0]  crc_out;             //crc结果
reg   [15:0]  crc_reg;             //crc寄存
reg   [15:0]  crc_table [0:255];   //CRC查找表
reg   [31:0]  data_in_reg [0:255]; //输入数据寄存
reg           write_en;            //数据输出使能
reg   [7:0]   history_memory_pick;
reg           write_finish;        //一次传输完毕
reg   [8:0]   wr_idx = 0;          //data_in_reg读
reg   [8:0]   rd_idx = 0;          //data_in_reg写
reg   [1:0]   work_state = 2'b00;
reg   [3:0]   dest_port_reg;
reg   [2:0]   data_priority_reg;

initial $readmemh("crc_table.txt",crc_table); 

integer i,j;

//--------输入数据寄存----------//
always@(posedge clk or negedge rst_n)
    if(!rst_n)begin
        wr_idx <= 'b0;
        for(j=0; j<256; j=j+1)
            data_in_reg[j] <= 32'd0;  
    end        
    else if(wr_vld == 1'b1)begin
        data_in_reg[wr_idx] <= data_in;
        wr_idx <= wr_idx + 1'b1;
    end
    else if((wr_eop == 1'b1) && (wr_vld == 1'b0))
        wr_idx <= 'b0;
    else 
        wr_idx <= wr_idx;

//---数据包长度计数---//
always@(posedge clk or negedge rst_n)
    if(rst_n == 1'b0)
        data_size_cnt <= 9'd0;
    else if(wr_vld == 1'b1)
        data_size_cnt <= data_size_cnt + 1'b1;
    else if(write_finish == 1'b1) //清零条件   
        data_size_cnt <= 9'd0;
    else 
        data_size_cnt <= data_size_cnt;      
        
//---需校验的有效数据（去掉包头）---//       
always@(posedge clk or negedge rst_n)begin
    if(rst_n == 1'b0)  
        data_real <= 32'd0;
    else if((wr_vld == 1'b1) && (data_size_cnt >= 9'd1))
        data_real <= data_in;
    else if((wr_vld == 1'b1) && (data_size_cnt == 9'd1))       
        data_real <= 32'd0;
    else if(crc_finish == 1'b1)
        data_real <= 32'd0;
    else
        data_real <= data_real;
end 

always@(posedge clk or negedge rst_n) //打一拍
    if(rst_n == 1'b0) 
        wr_eop_reg <= 1'b0;
    else
        wr_eop_reg <= wr_eop;
        
always@(posedge clk or negedge rst_n) //打一拍
    if(rst_n == 1'b0) 
        write_enable <= 1'b0;
    else
        write_enable <= write_en;

//--------取包头数据----------//        
always@(posedge clk or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        crc_in <= 16'b0;
        dest_port_reg <= 4'b0;
        data_priority_reg <= 3'b0;
    end 
    else if((wr_vld == 1'b1) && (data_size_cnt == 9'd0))begin
        crc_in <= data_in[22:7];
        dest_port_reg <= data_in[3:0];
        data_priority_reg <= data_in[6:4];    
    end 
    else if(write_en == 1'b1)begin  //清0条件
        crc_in <= 16'b0;
        dest_port_reg <= 4'b0;
        data_priority_reg <= 3'b0;
    end 
end    

always@(posedge clk or negedge rst_n)begin
    if(rst_n == 1'b0)begin
        dest_port <= 4'b0;
        data_priority <= 3'b0;
        data_size <= 11'b0;
        write_ready <= 1'b0;            
    end 
    else if(crc_true == 1'b1)begin
        dest_port <= dest_port_reg;
        data_priority <= data_priority_reg;
        data_size <= data_size_cnt*32;
        write_ready <= 1'b1; 
    end 
    else if((history_memory_pick & address_ready)>0) begin
        dest_port <= 4'b0;
        data_priority <= 3'b0;
        data_size <= 11'b0;
        write_ready <= 1'b0;          
    end
end        

//----------CRC校验----------//
always @(posedge clk or negedge rst_n) begin   //crc_reg不能清零？
    if (!rst_n) 
        crc_reg <= 16'hFFFF;
    else if((wr_vld||wr_eop)&&(data_size_cnt > 9'd1)) begin
        crc_reg <= crc_reg[15:8] ^ (crc_table[crc_reg[7:0] ^ data_real[31:24]]);
        crc_reg <= crc_reg[15:8] ^ (crc_table[crc_reg[7:0] ^ data_real[23:16]]);
        crc_reg <= crc_reg[15:8] ^ (crc_table[crc_reg[7:0] ^ data_real[15:8]]);
        crc_reg <= crc_reg[15:8] ^ (crc_table[crc_reg[7:0] ^ data_real[7:0]]); 
    end 
    else if(crc_finish == 1'b1)begin
        crc_reg <= 16'hFFFF;
        end 
end 

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) 
        crc_out <= 16'b0;
    else if(wr_eop_reg)
        crc_out <= crc_reg ^ 16'h0000;
    else if(write_finish == 1'b1)begin
        crc_out <= 16'b0;
        for(i=0; i<256; i=i+1)
            data_in_reg[i] <= 32'd0;
    end
    else
        crc_out <= crc_out;
end

always @(posedge clk or negedge rst_n) begin
    if (!rst_n)
        crc_finish <= 1'b0;
    else if(wr_eop_reg == 1'b1)
        crc_finish <= 1'b1;
    else 
        crc_finish <= 1'b0;
end 
//----------校验结束----------//

//----------判断crc结果----------//
always @(posedge clk or negedge rst_n) 
    if (!rst_n)
        crc_true <= 1'b0;
    else if(crc_finish == 1'b1)begin
            if(crc_out == crc_in)
                crc_true <= 1'b1; 
            else begin
                crc_true <= 1'b0;
                for(j=0; j<256; j=j+1)
                    data_in_reg[j] <= 32'd0; 
                end 
          end 
    else 
         crc_true <= 1'b0;
         
//---------data_wait_time---------//      
always @(posedge clk or negedge rst_n) 
    if (!rst_n)
        data_wait_time <= 10'b0;
    else if((crc_true == 1'b1) ||(write_ready == 1'b1))
        data_wait_time <= data_wait_time + 1'b1;
    else if(write_en == 1'b1)
        data_wait_time <= 'b0;
    else
        data_wait_time <= data_wait_time;            
        
//----------输出数据使能--------//
always@(posedge clk or negedge rst_n) 
    if (!rst_n)
        write_en <= 1'b0;
    else if((history_memory_pick & address_ready)>0)  //写使能拉高条件
        write_en <= 1'b1;
    else if(data_out_cnt == (data_size_cnt - 1'b1))
        write_en <= 1'b0;
    else
        write_en <= write_en;

//-----------输出数据计数-------//
always@(posedge clk or negedge rst_n) 
    if (!rst_n)
        data_out_cnt <= 9'b0;
    else if(write_en == 1'b1)
        data_out_cnt <= data_out_cnt + 1'b1;
    else if(data_out_cnt == data_size_cnt)
        data_out_cnt <= 9'b0;

//-----------传输数据结束-------//
always@(posedge clk or negedge rst_n) 
    if (!rst_n)
        write_finish <= 1'b0;
    else if((data_out_cnt == data_size_cnt) &&(data_out_cnt > 1'b1))
        write_finish <= 1'b1;
    else
        write_finish <= 1'b0;
 
//----------输出数据-----------// 
always @(posedge clk or negedge rst_n) 
    if (!rst_n)begin
        data_true <= 32'b0;
        rd_idx <= 'b0;
    end
    else if(write_en == 1'b1)begin
        data_true <= data_in_reg[rd_idx];
        rd_idx <= rd_idx + 1'b1;
    end
    else begin
        rd_idx <= 'b0;
        data_true <= 32'b0;
    end

//-----------状态转换----------//
always @(posedge clk or negedge rst_n) begin
    if (!rst_n)begin
        history_memory_pick <= 8'b0;
        work_state <= 2'b00;
    end 
    else if((write_arbition == 1'b1)&&(work_state == 2'b00))begin
        work_state <= 2'b01;
        history_memory_pick <= memory_pick;
    end 
    else if ((history_memory_pick & address_ready)>0)begin 
        work_state <= 2'b10;
        history_memory_pick <= 8'b0;
    end 
    else if(write_finish == 1'b1)
        work_state <= 2'b00;       
end


endmodule


