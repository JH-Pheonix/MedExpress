#include "asrpro.h"
#include <string.h>

asrpro_obj_t asrpro_init(uart_index_enum uartn, uart_rx_pin_enum rx_pin, uart_tx_pin_enum tx_pin, vuint32 baud)
{
    asrpro_obj_t asrpro;
    asrpro.uartn = uartn;
    memset(asrpro.rx_buf, 0, ASRPRO_RX_BUF_SIZE);
    asrpro.data_cnt = 0;
    asrpro.ack = 0;

    uart_init(uartn, baud, tx_pin, rx_pin);
    uart_rx_interrupt(uartn, 1);
    return asrpro;
}

static inline void discard_bytes(asrpro_obj_t *asrpro, int n)
{
    if (n <= 0)
        return;
    if (n >= asrpro->data_cnt)
    {
        asrpro->data_cnt = 0;
        return;
    }
    memmove(asrpro->rx_buf, asrpro->rx_buf + n, asrpro->data_cnt - n);
    asrpro->data_cnt -= n;
}

void asrpro_uart_handler(asrpro_obj_t *asrpro)
{
    vuint8 receive_data;
    if (uart_query_byte(asrpro->uartn, &receive_data))
    {
        if (receive_data == 0xA5 && asrpro->rx_buf[0] != 0xA5)
        {
            asrpro->data_cnt = 0;
        }

        if (asrpro->data_cnt < ASRPRO_RX_BUF_SIZE)
        {
            asrpro->rx_buf[asrpro->data_cnt++] = receive_data;
        }
        else
        {
            // 缓冲区溢出，重置状态
            asrpro->data_cnt = 0;
            return;
        }

        while (asrpro->data_cnt >= 4) // 最短帧为 header(1)+cmd(1)+len(1)+checksum(1)
        {
            // 确保头是 0xA5
            if (asrpro->rx_buf[0] != 0xA5)
            {
                // 在缓冲区中寻找下一个 0xA5 并把它移动到开头
                int idx = -1;
                for (int i = 1; i < asrpro->data_cnt; i++)
                {
                    if (asrpro->rx_buf[i] == 0xA5)
                    {
                        idx = i;
                        break;
                    }
                }

                if (idx == -1)
                {
                    // 没有找到新的帧头，丢弃所有数据
                    asrpro->data_cnt = 0;
                    return;
                }
                else
                {
                    // 找到新的帧头，移动数据
                    discard_bytes(asrpro, idx);
                }
            }

            if (asrpro->data_cnt < 4)
            {
                // 数据不足以构成最短帧，等待更多数据
                break;
            }

            vuint8 len = asrpro->rx_buf[2];
            if (len > (ASRPRO_RX_BUF_SIZE - 4))
            {
                // 非法长度，丢弃首字节继续解析
                discard_bytes(asrpro, 1);
                continue;
            }

            vuint8 need = 4 + len; // header + cmd + len + payload(len) + checksum
            if (need > ASRPRO_RX_BUF_SIZE)
            {
                // 不可能的情况，重置
                asrpro->data_cnt = 0;
                break;
            }

            if (asrpro->data_cnt < need)
            {
                // 不够完整帧，等待更多字节
                break;
            }

            // 计算校验和值
            vuint8 checksum = 0;
            for (vuint8 i = 0; i < (need - 1); i++)
            {
                checksum += asrpro->rx_buf[i];
            }

            if (checksum == asrpro->rx_buf[need - 1])
            {
                // 数据解析逻辑
                switch (asrpro->rx_buf[1])
                {
                case 0x03:
                    asrpro->ack = 1;
                    break;

                default:
                    break;
                }
                if (asrpro->data_cnt > need)
                {
                    // 使用 discard_bytes 一次性移除已解析的帧
                    discard_bytes(asrpro, need);
                    // 继续尝试解析下一个帧（如果有）
                    continue;
                }
                else
                {
                    // 刚好解析完所有字节
                    asrpro->data_cnt = 0;
                    break;
                }
            }
            else
            {
                // 校验失败，丢弃首字节继续尝试同步下一个帧头
                discard_bytes(asrpro, 1);
                continue;
            }
        }
    }
}

void asrpro_reset_ack(asrpro_obj_t *asrpro)
{
    asrpro->ack = 0;
}

vuint8 asrpro_get_ack(asrpro_obj_t *asrpro)
{
    return asrpro->ack;
}

void asrpro_set_status(asrpro_obj_t *asrpro, vuint8 status)
{
    switch (status)
    {
    case 1:
        while (asrpro->ack == 0)
        {
            // if (asrpro->uartn == UART_2)
            //     printf("one\n");
            // else
            //     printf("not uart2\n");
            uart_write_string(asrpro->uartn, "one\n");
            system_delay_ms(100);
        }
        break;

    case 2:
        while (asrpro->ack == 0)
        {
            uart_write_string(asrpro->uartn, "two\n");
            system_delay_ms(100);
        }
        break;

    case 3:
        while (asrpro->ack == 0)
        {
            uart_write_string(asrpro->uartn, "three\n");
            system_delay_ms(100);
        }
        break;

    default:
        break;
    }
}