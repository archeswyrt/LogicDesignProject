`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 12/18/2025 03:21:13 PM
// Design Name: 
// Module Name: SystemProject
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


module SystemProject(
    input  wire clk100,

    // SPI t? STM32
    input  wire spi_sck,
    input  wire spi_mosi,
    input  wire spi_cs_n,

    // Button h?c n?n
    input  wire btn,

    // Servo output
    output wire servo0,
    output wire servo1,
    output wire servo2,

    // Debug LED
    output wire led0,
    output wire led1,
    output wire led2,
    output wire led3,
    
    output wire led4_r,
    output wire led4_g,
    output wire led4_b  
);

    // ===== LED HOLD 2s =====
    reg [31:0] led_cnt;
    reg led_hold;
    reg led_r, led_g, led_b;

    localparam LED_2S = 200_000_000; // 2s @100MHz

    // =====================================================
    // SPI RX
    // =====================================================
    wire [7:0] rx_byte;
    wire rx_valid;

    spi_slave u_spi (
        .clk(clk100),
        .sck(spi_sck),
        .mosi(spi_mosi),
        .cs_n(spi_cs_n),
        .data_out(rx_byte),
        .data_valid(rx_valid)
    );
    
    // =====================================================
    // BACKGROUND SUBTRACTION
    // =====================================================
    wire pixel_valid;
    wire pixel_value;
    wire [7:0] px, py;
    wire frame_done;

    background_subtraction u_bg (
        .clk(clk100),
        .spi_cs_n(spi_cs_n),
        .rx_valid(rx_valid),
        .rx_byte(rx_byte),
        .learn_btn(btn),

        .pixel_valid(pixel_valid),
        .pixel_value(pixel_value),
        .px(px),
        .py(py),
        .frame_done(frame_done)
    );

    // =====================================================
    // MOMENT CALCULATION
    // =====================================================
    wire [31:0] M00, M10, M01, M11, M20, M02;
    wire moment_done;

    image_processing_core u_moment (
        .clk(clk100),
        .rst(spi_cs_n),
        .pixel_valid(pixel_valid),
        .pixel_value(pixel_value),
        .x(px),
        .y(py),
        .frame_done(frame_done),
        .M00(M00),
        .M10(M10),
        .M01(M01),
        .M11(M11),
        .M20(M20),
        .M02(M02),
        .done(moment_done)
    );

    // =====================================================
    // CLASSIFIER
    // =====================================================
    wire [1:0] shape_id;
    wire shape_valid;
    
    wire dbg_valid;
    wire is_circle;
    wire is_square;
    wire is_triangle;

    classifier u_cls (
        .clk(clk100),
        .moment_done(moment_done),
        .M00(M00),
        .M20(M20),
        .M02(M02),
        .shape_id(shape_id),
        .shape_valid(shape_valid),
        
        .dbg_valid(dbg_valid),
        .dbg_circle(is_circle),
        .dbg_square(is_square),
        .dbg_triangle(is_triangle)
    );

    // =====================================================
    // SERVO CONTROLLER
    // =====================================================
    wire start0, start1, start2;

    servo_controller u_servo_ctrl (
        .clk100(clk100),
        .shape_id(shape_id),
        .shape_valid(shape_valid),
        .start0(start0),
        .start1(start1),
        .start2(start2)
    );

    servo_cycle u_servo0 (.clk(clk100), .start(start0), .pwm_out(servo0));
    servo_cycle u_servo1 (.clk(clk100), .start(start1), .pwm_out(servo1));
    servo_cycle u_servo2 (.clk(clk100), .start(start2), .pwm_out(servo2));

    // =====================================================
    // DEBUG LED
    // =====================================================
    assign led0 = frame_done;
    assign led1 = moment_done;
    assign led2 = shape_valid;
    assign led3 = start0 | start1 | start2;
    
//    assign led4_r = is_circle;     
//    assign led4_g = is_square;     
//    assign led4_b = is_triangle;   
    
    assign led4_r = led_hold & led_r;
    assign led4_g = led_hold & led_g;
    assign led4_b = led_hold & led_b;

    
    // =====================================================
        // HOLD RGB LED FOR 2 SECONDS AFTER CLASSIFICATION
        // =====================================================
        always @(posedge clk100) begin
            if (shape_valid) begin
                // latch màu LED
                led_r <= is_circle;
                led_g <= is_square;
                led_b <= is_triangle;
    
                led_cnt  <= LED_2S;
                led_hold <= 1;
            end
            else if (led_hold) begin
                if (led_cnt > 0)
                    led_cnt <= led_cnt - 1;
                else
                    led_hold <= 0;
            end
        end
    
endmodule

//tách n?n
module background_subtraction (
    input  wire clk,
    input  wire spi_cs_n,
    input  wire rx_valid,
    input  wire [7:0] rx_byte,
    input  wire learn_btn,

    output wire pixel_valid,
    output wire pixel_value,
    output wire [7:0] px,
    output wire [7:0] py,
    output reg  frame_done
);

    reg [7:0] bg_mem [0:19199];
    reg [31:0] pixel_count;
    reg [7:0] diff;

    assign pixel_valid = rx_valid;
    assign px = pixel_count % 160;
    assign py = pixel_count / 160;
    assign pixel_value = (diff > 70);

    always @(posedge clk) begin
        if (spi_cs_n) begin
            pixel_count <= 0;
            frame_done <= 0;
        end
        else if (rx_valid) begin
            if (learn_btn)
                bg_mem[pixel_count] <= rx_byte;
            else begin
                diff <= (rx_byte > bg_mem[pixel_count]) ?
                        (rx_byte - bg_mem[pixel_count]) :
                        (bg_mem[pixel_count] - rx_byte);
            end

            if (pixel_count < 19199)
                pixel_count <= pixel_count + 1;
                
            if (pixel_count > 19000)
                frame_done <= 1;
            else 
                frame_done <= 0;
        end
    end
endmodule


//x? lí ?nh
module image_processing_core (
    input wire clk,
    input wire rst,
    input wire pixel_valid,
    input wire pixel_value,
    input wire [7:0] x,
    input wire [7:0] y,
    input wire frame_done,
    output reg [31:0] M00, M10, M01, M11, M20, M02,
    output reg done
);

    always @(posedge clk) begin
        if (rst) begin
            M00 <= 0; M10 <= 0; M01 <= 0;
            M11 <= 0; M20 <= 0; M02 <= 0;
            done <= 0;
        end
        else if (pixel_valid && pixel_value) begin
            M00 <= M00 + 1;
            M10 <= M10 + x;
            M01 <= M01 + y;
            M11 <= M11 + x*y;
            M20 <= M20 + x*x;
            M02 <= M02 + y*y;
        end
        else if (frame_done) begin
            done <= 1;
        end
        else if (done) begin
            M00 <= 0; M10 <= 0; M01 <= 0;
            M11 <= 0; M20 <= 0; M02 <= 0;
            done <= 0;
        end
    end
endmodule


//phân lo?i
module classifier (
    input wire clk,
    input wire moment_done,
    input wire [31:0] M00,
    input wire [31:0] M20,
    input wire [31:0] M02,
    output reg [1:0] shape_id,
    output reg shape_valid,
    
    // ===== DEBUG =====
    output reg dbg_valid,
    output reg dbg_circle,
    output reg dbg_square,
    output reg dbg_triangle
);

    always @(posedge clk) begin
        shape_valid  <= 0;
        dbg_valid    <= 0;
        dbg_circle   <= 0;
        dbg_square   <= 0;
        dbg_triangle <= 0;
        if (moment_done) begin
        if (M00 < 200) begin
            shape_valid <= 0;   // KHÔNG CÓ V?T
        end
        dbg_valid <= 1;
            if ((M20 > M02 - 1000) && (M20 < M02 + 1000)) begin
                shape_id <= 2'd0;   // circle
                dbg_circle   <= 1;
            end
            else if (M00 > 5000) begin
                shape_id <= 2'd1;   // square
                dbg_square   <= 1;
            end
            else begin
                shape_id <= 2'd2;   // triangle
                dbg_triangle <= 1;
            end
            shape_valid <= 1;
        end
    end
endmodule


//?i?u khi?n servo
module servo_controller (
    input wire clk100,
    input wire [1:0] shape_id,
    input wire shape_valid,
    output reg start0,
    output reg start1,
    output reg start2
);

    localparam D2 = 200_000_000;
    localparam D4 = 400_000_000;
    localparam D6 = 600_000_000;

    reg [31:0] cnt;
    reg [1:0] cmd;
    reg busy;
    
    initial begin
        cnt  = 0;
        cmd  = 0;
        busy = 0;
    end
    
    always @(posedge clk100) begin
        start0 <= 0; start1 <= 0; start2 <= 0;

        if (shape_valid && !busy) begin
            cmd <= shape_id;
            cnt <= 0;
            busy <= 1;
        end

        if (busy) begin
            cnt <= cnt + 1;
            case (cmd)
                2'd0: if (cnt == D2) begin start0 <= 1; busy <= 0; end
                2'd1: if (cnt == D4) begin start1 <= 1; busy <= 0; end
                2'd2: if (cnt == D6) begin start2 <= 1; busy <= 0; end
            endcase
        end
        if (!shape_valid) begin
                busy <= 0;
        end
    end
endmodule



//?i?u khi?n servo
module servo_cycle (
    input  wire clk,        // 100 MHz
    input  wire start,      // start pulse
    output reg  pwm_out
);

    // ---------- PWM CONFIG ----------
    parameter PWM_PERIOD = 2000000;
    parameter PULSE_180  = 280000;
    parameter PULSE_90   = 187500;
    parameter PULSE_0    = 50000;

    // ---------- TIME (3s cho 1 chi?u) ----------
    parameter THREE_SEC = 75000000;   // 3s @100MHz
    parameter CYCLES_3S = THREE_SEC / PWM_PERIOD;

    // ---------- STATES ----------
    localparam ST_IDLE   = 2'd0;
    localparam ST_GO_90 = 2'd1;
    localparam ST_GO_180   = 2'd2;

    reg [1:0] state = ST_IDLE;

    reg [31:0] pwm_cnt   = 0;
    reg [31:0] cycle_cnt = 0;
    reg [31:0] pulse_width = PULSE_0;

    // =====================================================
    always @(posedge clk) begin

        // ---------- PWM ----------
        if (pwm_cnt < PWM_PERIOD)
            pwm_cnt <= pwm_cnt + 1;
        else begin
            pwm_cnt <= 0;
            cycle_cnt <= cycle_cnt + 1;
        end

        pwm_out <= (pwm_cnt < pulse_width);

        // ---------- FSM ----------
        case (state)

    // ===== IDLE: gi? 180° =====
    ST_IDLE: begin
        pulse_width <= PULSE_180;
        cycle_cnt   <= 0;
        if (start)
            state <= ST_GO_90;
    end

    // ===== 180° ? 90° =====
    ST_GO_90: begin
        pulse_width <= PULSE_180 +
            ((PULSE_90 - PULSE_180) * cycle_cnt) / CYCLES_3S;

        if (cycle_cnt >= CYCLES_3S) begin
            cycle_cnt <= 0;
            state     <= ST_GO_180;
        end
    end

    // ===== 90° ? 180° =====
    ST_GO_180: begin
        pulse_width <= PULSE_90 +
            ((PULSE_180 - PULSE_90) * cycle_cnt) / CYCLES_3S;

        if (cycle_cnt >= CYCLES_3S) begin
            cycle_cnt <= 0;
            state     <= ST_IDLE;
        end
    end

endcase
    end
endmodule
 
//module uart_tx (
//    input wire clk,
//    input wire start,
//    input wire [7:0] data,
//    output reg tx,
//    output wire busy
//);
//    // T?c ?? 115200 baud @ 100MHz clock
//    parameter CLKS_PER_BIT = 868; 
    
//    reg [2:0] state = 0;
//    reg [15:0] clk_count = 0;
//    reg [2:0] bit_index = 0;
//    reg [7:0] data_reg = 0;

//    assign busy = (state != 0);

//    always @(posedge clk) begin
//        case (state)
//            0: begin // IDLE
//                tx <= 1;
//                if (start) begin
//                    state <= 1;
//                    data_reg <= data;
//                    clk_count <= 0;
//                end
//            end
//            1: begin // START BIT
//                tx <= 0;
//                if (clk_count < CLKS_PER_BIT-1) clk_count <= clk_count + 1;
//                else begin clk_count <= 0; state <= 2; bit_index <= 0; end
//            end
//            2: begin // DATA BITS
//                tx <= data_reg[bit_index];
//                if (clk_count < CLKS_PER_BIT-1) clk_count <= clk_count + 1;
//                else begin 
//                    clk_count <= 0;
//                    if (bit_index < 7) bit_index <= bit_index + 1;
//                    else state <= 3;
//                end
//            end
//            3: begin // STOP BIT
//                tx <= 1;
//                if (clk_count < CLKS_PER_BIT-1) clk_count <= clk_count + 1;
//                else state <= 0;
//            end
//        endcase
//    end
//endmodule

