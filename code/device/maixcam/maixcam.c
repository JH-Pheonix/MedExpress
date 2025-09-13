#include "maixcam.h"

maixcam_message_t data;

maixcam_obj_t maixcam_uart_init(uart_index_enum uartn, uart_rx_pin_enum rx_pin, uart_tx_pin_enum tx_pin, vuint32 baud)
{
    maixcam_obj_t maixcam;
    maixcam.uartn = uartn;

    memset(maixcam.rx_buf, 0, MAIXCAM_RX_BUF_SIZE);
    maixcam.data_cnt = 0;

    data.cmd = 0;
    data.data = 0;

    uart_init(uartn, baud, tx_pin, rx_pin);
    uart_rx_interrupt(uartn, 1); // 使能串口接收中断

    return maixcam;
}

void maixcam_uart_handler(maixcam_obj_t *maixcam)
{
    vuint8 receive_data;
    if (uart_query_byte(maixcam->uartn, &receive_data))
    {
        if (receive_data == 0xA5 && maixcam->rx_buf[0] != 0xA5)
        {
            maixcam->data_cnt = 0;
        }

        maixcam->rx_buf[maixcam->data_cnt++] = receive_data;

        if (maixcam->data_cnt >= 5)
        {
            if (maixcam->rx_buf[0] == 0xA5)
            {
                vuint8 checksum = 0;
                for (vuint8 i = 0; i < 4; i++)
                {
                    checksum += maixcam->rx_buf[i];
                }

                if (checksum == maixcam->rx_buf[4])
                {
                    switch (maixcam->rx_buf[1])
                    {
                    case 0x00:
                        data.cmd = maixcam->rx_buf[1];
                        data.data = maixcam->rx_buf[2];
                        break;
                    case 0x01:
                        data.cmd = maixcam->rx_buf[1];
                        data.data = maixcam->rx_buf[2];
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }
}

maixcam_message_t maixcam_pop_data()
{
    return data;
}

void maixcam_clear() {
    data.cmd = 0;
    data.data = 0;
}

