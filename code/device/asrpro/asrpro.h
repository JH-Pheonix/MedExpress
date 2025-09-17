#ifndef _DEVICE_ASRPRO_H_
#define _DEVICE_ASRPRO_H_

#include "zf_driver_uart.h"

#define ASRPRO_RX_BUF_SIZE 64

typedef struct
{
    uart_index_enum uartn;
    vuint8 rx_buf[ASRPRO_RX_BUF_SIZE];
    vuint16 data_cnt;
    vuint8 ack;
} asrpro_obj_t;

asrpro_obj_t asrpro_init(uart_index_enum uartn, uart_rx_pin_enum rx_pin, uart_tx_pin_enum tx_pin, vuint32 baud);
void asrpro_uart_handler(asrpro_obj_t *asrpro);
void asrpro_reset_ack(asrpro_obj_t *asrpro);
vuint8 asrpro_get_ack(asrpro_obj_t *asrpro);
void asrpro_set_status(asrpro_obj_t *asrpro, vuint8 status);

#endif