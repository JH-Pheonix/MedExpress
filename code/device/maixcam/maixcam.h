#ifndef _DEVICE_MAIXCAM_H_
#define _DEVICE_MAIXCAM_H_

#include "zf_driver_uart.h"
#include "zf_driver_gpio.h"
#include "zf_common_typedef.h"

#define MAIXCAM_RX_BUF_SIZE 64
#define MAIXCAM_PAYLOAD_MAX 60

typedef struct
{
    uart_index_enum uartn;
    vuint8 rx_buf[MAIXCAM_RX_BUF_SIZE];
    vuint8 data_cnt;
    vuint8 receive_flag;
} maixcam_obj_t;

typedef struct
{
    vuint8 cmd;
    vuint8 len; // payload 长度
    vuint64 data;
} maixcam_message_t;

maixcam_obj_t maixcam_uart_init(uart_index_enum uartn, uart_rx_pin_enum rx_pin, uart_tx_pin_enum tx_pin, vuint32 baud);
void maixcam_uart_handler(maixcam_obj_t *maixcam);
maixcam_message_t maixcam_pop_data();
void maixcam_clear(maixcam_obj_t *maixcam);

#endif
