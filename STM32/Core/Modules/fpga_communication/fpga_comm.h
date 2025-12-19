/*
 * fpga_comm.h
 *
 *  Created on: Dec 14, 2025
 *      Author: Nam Truong
 */

#ifndef MODULES_FPGA_COMMUNICATION_FPGA_COMM_H_
#define MODULES_FPGA_COMMUNICATION_FPGA_COMM_H_


#include "main.h"

// Định nghĩa kích thước ảnh gửi đi
#define FPGA_IMG_WIDTH  160
#define FPGA_IMG_HEIGHT 120

/*
 * Hàm gửi buffer ảnh RGB565 sang FPGA.
 * Bên trong hàm sẽ tự động chuyển đổi sang Grayscale để gửi đi.
 */
void FPGA_Send_Image(uint8_t *gray__buffer, uint8_t width, uint8_t height);

#endif /* MODULES_FPGA_COMMUNICATION_FPGA_COMM_H_ */
