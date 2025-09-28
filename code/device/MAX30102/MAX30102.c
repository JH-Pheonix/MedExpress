#include "MAX30102.h"

MAX30102_obj_t MAX30102_init(gpio_pin_enum scl_pin, gpio_pin_enum sda_pin, MAX30102_mode_e mode)
{
    MAX30102_obj_t MAX30102_obj;

    soft_iic_init(&MAX30102_obj.MAX30102_iic_obj, MAX30102_ADDR, MAX30102_IIC_DELAY, scl_pin, sda_pin);

    system_delay_ms(1000);
    // if (!MAX30102_check_part_id(&MAX30102_obj))
    // {
    //     printf("MAX30102 init failed!\n");
    //     return MAX30102_obj;
    // };

    MAX30102_reset(&MAX30102_obj);
    system_delay_ms(100); // 复位后等待 100ms

    MAX30102_obj.mode = mode;
    MAX30102_config_mode(&MAX30102_obj);
    MAX30102_config_spo2(&MAX30102_obj);
    MAX30102_config_fifo(&MAX30102_obj);
    MAX30102_config_led(&MAX30102_obj);

    return MAX30102_obj;
}

void MAX30102_reset(MAX30102_obj_t *MAX30102_obj)
{
    soft_iic_write_8bit_register(&MAX30102_obj->MAX30102_iic_obj, 0x01, 0x40);
    while (soft_iic_read_8bit_register(&MAX30102_obj->MAX30102_iic_obj, REG_MODE_CONFIG) & 0x40)
    {
        system_delay_ms(1);
    }
}

void MAX30102_read_fifo(MAX30102_obj_t *MAX30102_obj)
{
    vuint8 fifo_data[6];
    switch (MAX30102_obj->mode)
    {
    case MODE_HR_ONLY:
        soft_iic_read_8bit_registers(&MAX30102_obj->MAX30102_iic_obj, REG_FIFO_DATA, fifo_data, 3);
        MAX30102_obj->ir = ((vuint32)fifo_data[0] << 16) | ((vuint32)fifo_data[1] << 8) | (vuint32)fifo_data[2];
        MAX30102_obj->red = 0;
        // printf("111111111\n");
        break;
    case MODE_SPO2_HR:
        soft_iic_read_8bit_registers(&MAX30102_obj->MAX30102_iic_obj, REG_FIFO_DATA, fifo_data, 6);
        MAX30102_obj->ir = ((vuint32)fifo_data[0] << 16) | ((vuint32)fifo_data[1] << 8) | (vuint32)fifo_data[2];
        MAX30102_obj->red = ((vuint32)fifo_data[3] << 16) | ((vuint32)fifo_data[4] << 8) | (vuint32)fifo_data[5];
        // printf("222222222\n");
        break;
    default:
        break;
    }
}

void MAX30102_config_mode(MAX30102_obj_t *MAX30102_obj)
{
    soft_iic_write_8bit_register(&MAX30102_obj->MAX30102_iic_obj, REG_MODE_CONFIG, MAX30102_obj->mode);
}

void MAX30102_config_spo2(MAX30102_obj_t *MAX30102_obj)
{
    vuint8 spo2_config = SPO2_ADC_RANGE_2048 |
                         SPO2_ADC_RANGE_16384 |
                         LED_PULSE_WIDTH_411US;
    soft_iic_write_8bit_register(&MAX30102_obj->MAX30102_iic_obj, REG_SPO2_CONFIG, spo2_config);
}

void MAX30102_config_fifo(MAX30102_obj_t *MAX30102_obj)
{
    vuint8 fifo_config = SAMPLE_AVERAGING_1X |  // 4 个数据取平均
                         FIFO_ROLLOVER_ENABLE | // 满了继续覆盖
                         FIFO_ALMOST_FULL_2;    // 剩 2 个空间时中断
    soft_iic_write_8bit_register(&MAX30102_obj->MAX30102_iic_obj, REG_FIFO_CONFIG, fifo_config);
    // soft_iic_write_8bit_register(&MAX30102_obj->MAX30102_iic_obj, REG_INTR_ENABLE_1, 0x01); // 使能 FIFO 满中断
}

void MAX30102_config_led(MAX30102_obj_t *MAX30102_obj)
{
    soft_iic_write_8bit_register(&MAX30102_obj->MAX30102_iic_obj, REG_LED1_PA, LED_BRIGHTNESS_HIGH);
    soft_iic_write_8bit_register(&MAX30102_obj->MAX30102_iic_obj, REG_LED2_PA, LED_BRIGHTNESS_MAX);
}

vuint8 MAX30102_data_ready(MAX30102_obj_t *MAX30102_obj)
{
    vuint8 wr_ptr, rd_ptr;
    rd_ptr = soft_iic_read_8bit_register(&MAX30102_obj->MAX30102_iic_obj, REG_FIFO_RD_PTR);
    wr_ptr = soft_iic_read_8bit_register(&MAX30102_obj->MAX30102_iic_obj, REG_FIFO_WR_PTR);
    vuint8 count = (wr_ptr - rd_ptr) & 0x1F;
    return count > 0;
}
