#ifndef _DEVICE_MAX30102_H_
#define _DEVICE_MAX30102_H_

#include "zf_driver_soft_iic.h"
#include "zf_common_typedef.h"
#include "zf_driver_gpio.h"

// MAX30102 7-bit I2C 地址
#define MAX30102_I2C_ADDR 0x57

// 寄存器地址（基于 MAX30102 数据手册常用定义）
#define MAX30102_REG_INT_STATUS 0x00
#define MAX30102_REG_INT_ENABLE 0x01
#define MAX30102_REG_FIFO_WR_PTR 0x04
#define MAX30102_REG_OVF_COUNTER 0x05
#define MAX30102_REG_FIFO_RD_PTR 0x06
#define MAX30102_REG_FIFO_DATA 0x07
#define MAX30102_REG_MODE_CONFIG 0x09
#define MAX30102_REG_SPO2_CONFIG 0x0A
#define MAX30102_REG_LED1_PULSE 0x0C // LED1 (RED) Pulse Amplitude
#define MAX30102_REG_LED2_PULSE 0x0D // LED2 (IR) Pulse Amplitude

// 温度寄存器（器件内部温度）
#define MAX30102_REG_TEMP_INT 0x1F
#define MAX30102_REG_TEMP_FRAC 0x20

// MODE_CONFIG bits
#define MAX30102_MODE_RESET 0x40

typedef struct
{
    soft_iic_info_struct i2c; // 内含 scl/sda/addr/delay
} MAX30102_struct;

// 初始化，传入 SCL/SDA 引脚枚举（gpio_pin_enum）
void MAX30102_init(MAX30102_struct *dev, gpio_pin_enum scl_pin, gpio_pin_enum sda_pin);

// 读写寄存器
void MAX30102_write_reg(MAX30102_struct *dev, vuint8 reg, vuint8 val);
vuint8 MAX30102_read_reg(MAX30102_struct *dev, vuint8 reg);
void MAX30102_read_regs(MAX30102_struct *dev, vuint8 reg, vuint8 *buf, vuint32 len);

// FIFO 操作
// 读取 FIFO，一次读取 red 和 ir 各一个样本，放入 uint32（18-bit 整数）
typedef struct
{
    vuint32 red;
    vuint32 ir;
} MAX30102_sample_t;

// 返回一个样本（red, ir）
MAX30102_sample_t MAX30102_read_fifo(MAX30102_struct *dev);
// 清空 FIFO 指针和溢出计数
void MAX30102_clear_fifo(MAX30102_struct *dev);

// 复位设备（软件复位）
void MAX30102_reset(MAX30102_struct *dev);

// 简单控制函数
void MAX30102_set_mode(MAX30102_struct *dev, vuint8 mode);
void MAX30102_set_led_pa(MAX30102_struct *dev, vuint8 led1_pa, vuint8 led2_pa);

// 读取内部温度（摄氏度）
float MAX30102_read_temperature(MAX30102_struct *dev);

#endif