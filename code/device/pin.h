#ifndef _DEVICE_PIN_H_
#define _DEVICE_PIN_H_

// ADIS pin
#define ADIS_SPI (SPI_1)
#define ADIS_CS_PIN (B19)
#define ADIS_RST_PIN (B10)
#define ADIS_MOSI (SPI1_MOSI_B22)
#define ADIS_MISO (SPI1_MISO_B21)
#define ADIS_SCK (SPI1_SCK_B23)
#define ADIS_SPEED (2 * 1000 * 1000)

// tft180 pin
#define LCD_SPI (SPI_1)
#define LCD_SCL_PIN (SPI1_SCK_B9)
#define LCD_SDA_PIN (SPI1_MOSI_B8)
#define LCD_RES_PIN (B10) // 复位引脚
#define LCD_CS_PIN (B14)  // 片选引脚
#define LCD_DC_PIN (B11)  // 数据/命令引脚
#define LCD_BL_PIN (B26)  // 背光引脚

// key pin
//                A    , C    , B    , D    , CENTER
#define MKEY_NAME KEY_U, KEY_D, KEY_L, KEY_R, KEY_B
#define MKEY_LIST A16, A30, A15, A31, A29

#endif