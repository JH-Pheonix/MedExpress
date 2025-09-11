#include "MAX30102.h"

// 初始化设备结构并配置软 I2C
void MAX30102_init(MAX30102_struct *dev, gpio_pin_enum scl_pin, gpio_pin_enum sda_pin)
{
    // 初始化软 I2C，使用默认地址和小延时
    soft_iic_init(&dev->i2c, MAX30102_I2C_ADDR, 50, scl_pin, sda_pin);
}

// 写寄存器
void MAX30102_write_reg(MAX30102_struct *dev, vuint8 reg, vuint8 val)
{
    soft_iic_write_8bit_register(&dev->i2c, reg, val);
}

// 读寄存器
vuint8 MAX30102_read_reg(MAX30102_struct *dev, vuint8 reg)
{
    return soft_iic_sccb_read_register(&dev->i2c, reg);
}

// 连续读寄存器
void MAX30102_read_regs(MAX30102_struct *dev, vuint8 reg, vuint8 *buf, vuint32 len)
{
    soft_iic_read_8bit_registers(&dev->i2c, reg, buf, len);
}

// 读取 FIFO（每次读取两个通道数据：RED, IR）
// MAX30102 FIFO 数据为 3 bytes per sample per channel, 18-bit
MAX30102_sample_t MAX30102_read_fifo(MAX30102_struct *dev)
{
    MAX30102_sample_t sample;
    vuint8 buf[6];
    // 从 FIFO_DATA 读 6 字节（RED MSB->LSB, IR MSB->LSB）
    soft_iic_read_8bit_registers(&dev->i2c, MAX30102_REG_FIFO_DATA, buf, 6);

    sample.red = ((vuint32)(buf[0] & 0x03) << 16) | ((vuint32)buf[1] << 8) | buf[2];
    sample.ir = ((vuint32)(buf[3] & 0x03) << 16) | ((vuint32)buf[4] << 8) | buf[5];

    return sample;
}

// 清空 FIFO 指针
void MAX30102_clear_fifo(MAX30102_struct *dev)
{
    // 将 WR_PTR, OVF_COUNTER, RD_PTR 置 0
    MAX30102_write_reg(dev, MAX30102_REG_FIFO_WR_PTR, 0);
    MAX30102_write_reg(dev, MAX30102_REG_OVF_COUNTER, 0);
    MAX30102_write_reg(dev, MAX30102_REG_FIFO_RD_PTR, 0);
}

// 软件复位
void MAX30102_reset(MAX30102_struct *dev)
{
    MAX30102_write_reg(dev, MAX30102_REG_MODE_CONFIG, MAX30102_MODE_RESET);
}

// 设置工作模式（例如 0x03 为 SpO2 模式）
void MAX30102_set_mode(MAX30102_struct *dev, vuint8 mode)
{
    MAX30102_write_reg(dev, MAX30102_REG_MODE_CONFIG, mode);
}

// 设置 LED 脉冲幅度（0-255）
void MAX30102_set_led_pa(MAX30102_struct *dev, vuint8 led1_pa, vuint8 led2_pa)
{
    MAX30102_write_reg(dev, MAX30102_REG_LED1_PULSE, led1_pa);
    MAX30102_write_reg(dev, MAX30102_REG_LED2_PULSE, led2_pa);
}

// 读取内部温度（摄氏度）
float MAX30102_read_temperature(MAX30102_struct *dev)
{
    // 根据 MAX30102 手册，温度寄存器通常为整数和小数寄存器
    vuint8 t_int = MAX30102_read_reg(dev, MAX30102_REG_TEMP_INT);
    vuint8 t_frac = MAX30102_read_reg(dev, MAX30102_REG_TEMP_FRAC);

    // 整数部分为带符号 8-bit（补码）
    int8 signed_int = (int8)t_int;

    // 小数部分通常使用高 4 位，每 bit = 0.0625 C
    vuint8 frac_nibble = (t_frac >> 4) & 0x0F;
    float frac = (float)frac_nibble * 0.0625f;

    float temp = (float)signed_int + frac;
    return temp;
}
