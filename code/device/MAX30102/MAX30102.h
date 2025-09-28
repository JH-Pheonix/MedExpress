#ifndef MAX30102_H
#define MAX30102_H

#include "zf_common_typedef.h"
#include "zf_driver_soft_iic.h"
#include "zf_driver_gpio.h"

#define MAX30102_ADDR 0x57
#define MAX30102_IIC_DELAY 100 // IIC 时钟延时参数,值越大,时钟频率越低
#define MAX30102_PART_ID 0x15

// 采样平均
#define SAMPLE_AVERAGING_1X (0x00 << 5)  // 1 sample
#define SAMPLE_AVERAGING_2X (0x01 << 5)  // 2 samples
#define SAMPLE_AVERAGING_4X (0x02 << 5)  // 4 samples
#define SAMPLE_AVERAGING_8X (0x03 << 5)  // 8 samples
#define SAMPLE_AVERAGING_16X (0x04 << 5) // 16 samples
#define SAMPLE_AVERAGING_32X (0x05 << 5) // 32 samples (0b101, 110, 111 also 32x)

// FIFO 滚动覆盖
#define FIFO_ROLLOVER_ENABLE 0x10 // BIT4
#define FIFO_ROLLOVER_DISABLE 0x00

// FIFO Almost Full 阈值（剩余空间）
#define FIFO_ALMOST_FULL_1 0x01  // 剩 1 个空间时中断
#define FIFO_ALMOST_FULL_2 0x02  // 剩 2 个空间时中断
#define FIFO_ALMOST_FULL_15 0x0F // 剩 15 个空间时中断

// SPO2 配置
// ADC 满量程
#define SPO2_ADC_RANGE_2048 (0x00 << 5)  // 2048 nA, 7.81 pA/LSB
#define SPO2_ADC_RANGE_4096 (0x01 << 5)  // 4096 nA, 15.63 pA/LSB
#define SPO2_ADC_RANGE_8192 (0x02 << 5)  // 8192 nA, 31.25 pA/LSB
#define SPO2_ADC_RANGE_16384 (0x03 << 5) // 16384 nA, 62.5 pA/LSB

// SPO2 采样率配置
#define SPO2_SAMPLE_RATE_50 (0x00 << 2)   // 50 Hz
#define SPO2_SAMPLE_RATE_100 (0x01 << 2)  // 100 Hz
#define SPO2_SAMPLE_RATE_200 (0x02 << 2)  // 200 Hz
#define SPO2_SAMPLE_RATE_400 (0x03 << 2)  // 400 Hz
#define SPO2_SAMPLE_RATE_800 (0x04 << 2)  // 800 Hz
#define SPO2_SAMPLE_RATE_1000 (0x05 << 2) // 1000 Hz
#define SPO2_SAMPLE_RATE_1600 (0x06 << 2) // 1600 Hz
#define SPO2_SAMPLE_RATE_3200 (0x07 << 2) // 3200 Hz

#define LED_PULSE_WIDTH_69US (0x00 << 0)  // 69 µs, 15-bit resolution
#define LED_PULSE_WIDTH_118US (0x01 << 0) // 118 µs, 16-bit resolution
#define LED_PULSE_WIDTH_215US (0x02 << 0) // 215 µs, 17-bit resolution
#define LED_PULSE_WIDTH_411US (0x03 << 0) // 411 µs, 18-bit resolution

// LED 电流 (0–255, 实际电流与外部电路有关)
// MAX30102 通过 红光（RED）和红外（IR）LED 照射手指，光线穿透组织后被光电探测器接收。
// LED 发光越强, 探测器接收到的光越多, 信号幅度越大
// 调试的时候，先调 IR 电流，看到稳定脉搏波形后，再调 RED 电流
// 配合 ADC 满量程使用, 信号太弱时增大 LED 电流 或 减小 ADC 满量程（如 4096nA）。
// 减小满量程会提高灵敏度但缩小动态范围
#define LED_BRIGHTNESS_LOW 0x1F
#define LED_BRIGHTNESS_MEDIUM 0x3F
#define LED_BRIGHTNESS_HIGH 0x7F
#define LED_BRIGHTNESS_MAX 0xFF

typedef enum
{
    MODE_POWER_DOWN = 0x00, // 省电模式（Power Down Mode）
    MODE_RESERVED,          // 保留
    MODE_HR_ONLY,           // 仅心率模式（Heart Rate Only Mode，只看ir）
    MODE_SPO2_HR,           // 血氧 + 心率模式（SpO₂ and HR Mode）
    MODE_MULTI_LED,         // 多 LED 模式（Multi-LED Mode）, MAX30102 实际只支持 2 个。实际上一般不用这个东西
} MAX30102_mode_e;

typedef enum
{
    REG_INTR_STATUS_1 = 0x00,
    REG_INTR_STATUS_2,
    REG_INTR_ENABLE_1,
    REG_INTR_ENABLE_2,
    REG_FIFO_WR_PTR,
    REG_OVF_COUNTER,
    REG_FIFO_RD_PTR,
    REG_FIFO_DATA,
    REG_FIFO_CONFIG,
    REG_MODE_CONFIG,
    REG_SPO2_CONFIG,    // 0x0A
    REG_LED1_PA = 0x0C, // Red 红光 LED
    REG_LED2_PA,        // IR 红外 LED
    REG_DIE_TEMP_INT = 0X1F,
    REG_DIE_TEMP_FRAC = 0x20,
    REG_DIE_TEMP_CONFIG,
    REG_PART_ID = 0xFF,
} MAX30102_register_addr_e;

typedef struct
{
    soft_iic_info_struct MAX30102_iic_obj;
    vuint32 red;
    vuint32 ir;
    MAX30102_mode_e mode;
} MAX30102_obj_t;

MAX30102_obj_t MAX30102_init(gpio_pin_enum scl_pin, gpio_pin_enum sda_pin, MAX30102_mode_e mode);
void MAX30102_reset(MAX30102_obj_t *MAX30102_obj);
vuint8 MAX30102_data_ready(MAX30102_obj_t *MAX30102_obj);
void MAX30102_read_fifo(MAX30102_obj_t *MAX30102_obj);
void MAX30102_config_mode(MAX30102_obj_t *MAX30102_obj);
void MAX30102_config_spo2(MAX30102_obj_t *MAX30102_obj);
void MAX30102_config_fifo(MAX30102_obj_t *MAX30102_obj);
void MAX30102_config_led(MAX30102_obj_t *MAX30102_obj);

#endif