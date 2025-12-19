`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 12/17/2025 01:25:15 AM
// Design Name: 
// Module Name: spi_slave
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module spi_slave(
    input wire clk,         // Clock hệ thống (100MHz từ board)
    input wire sck,         // Từ STM32
    input wire mosi,        // Từ STM32
    input wire cs_n,        // Từ STM32 (Chip Select, Active Low)
    output reg [7:0] data_out, // Byte nhận được
    output reg data_valid      // Cờ báo "Đã nhận xong 1 byte"
);

    reg [2:0] sck_r;
    reg [2:0] cs_n_r;
    reg [2:0] mosi_r;

    // Đồng bộ hóa tín hiệu để tránh nhiễu (CDC)
    always @(posedge clk) begin
        sck_r  <= {sck_r[1:0], sck};
        cs_n_r <= {cs_n_r[1:0], cs_n};
        mosi_r <= {mosi_r[1:0], mosi};
    end

    wire sck_rising  = (sck_r[2:1] == 2'b01); // Phát hiện cạnh lên SCK
    wire cs_active   = (cs_n_r[1] == 1'b0);   // CS đang thấp

    reg [2:0] bit_cnt;
    reg [7:0] shift_reg;

    always @(posedge clk) begin
        if (!cs_active) begin
            bit_cnt <= 0;
            data_valid <= 0;
        end else if (sck_rising) begin
            shift_reg <= {shift_reg[6:0], mosi_r[1]}; // Dịch bit vào
            bit_cnt <= bit_cnt + 1;
            if (bit_cnt == 7) begin // Đủ 8 bit
                data_out <= {shift_reg[6:0], mosi_r[1]};
                data_valid <= 1;
            end else begin
                data_valid <= 0;
            end
        end else begin
            data_valid <= 0;
        end
    end
endmodule
