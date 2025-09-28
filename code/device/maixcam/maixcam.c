#include "maixcam.h"
#include <string.h>

maixcam_message_t data;

static vuint64 parse_payload(maixcam_obj_t *maixcam, vuint8 cmd, vuint8 *payload, vuint8 len)
{
    switch (cmd)
    {
    case 0x00:
    {
        maixcam->receive_flag = 1;
        if (len == 1)
        {
            return (vuint64)payload[0];
        }
        return 0;
    }
    case 0x01:
    {
        maixcam->receive_flag = 1;
        vuint64 v = 0;
        for (int i = 0; i < len; i++)
            v = (v << 8) | (vuint64)payload[i];
        return v;
    }
    default:
        return 0;
    }
}

static inline void discard_bytes(maixcam_obj_t *maixcam, int n)
{
    if (n <= 0)
        return;
    if (n >= maixcam->data_cnt)
    {
        maixcam->data_cnt = 0;
        return;
    }
    memmove(maixcam->rx_buf, maixcam->rx_buf + n, maixcam->data_cnt - n);
    maixcam->data_cnt -= n;
}

maixcam_obj_t maixcam_uart_init(uart_index_enum uartn, uart_rx_pin_enum rx_pin, uart_tx_pin_enum tx_pin, vuint32 baud)
{
    maixcam_obj_t maixcam;
    maixcam.uartn = uartn;

    memset(maixcam.rx_buf, 0, MAIXCAM_RX_BUF_SIZE);
    maixcam.data_cnt = 0;
    maixcam.receive_flag = 0;

    data.cmd = 0;
    data.len = 0;
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

        if (maixcam->data_cnt < MAIXCAM_RX_BUF_SIZE)
        {
            maixcam->rx_buf[maixcam->data_cnt++] = receive_data;
        }
        else
        {
            // 缓冲区溢出，重置状态
            maixcam->data_cnt = 0;
            return;
        }

        // 尝试解析尽可能多的完整帧（处理粘包）
        while (maixcam->data_cnt >= 4) // 最短帧为 header(1)+cmd(1)+len(1)+checksum(1)
        {
            // 确保头是 0xA5
            if (maixcam->rx_buf[0] != 0xA5)
            {
                // 在缓冲区中寻找下一个 0xA5 并把它移动到开头
                int idx = -1;
                for (int i = 1; i < maixcam->data_cnt; i++)
                {
                    if (maixcam->rx_buf[i] == 0xA5)
                    {
                        idx = i;
                        break;
                    }
                }

                if (idx == -1)
                {
                    // 未找到，清空缓冲区
                    maixcam->data_cnt = 0;
                    break;
                }

                // 将剩余数据左移
                discard_bytes(maixcam, idx);

                if (maixcam->data_cnt < 4)
                    break; // 数据不足以构成最短帧，等待更多字节
            }

            vuint8 len = maixcam->rx_buf[2];
            if (len > MAIXCAM_PAYLOAD_MAX)
            {
                // 非法长度，丢弃首字节继续解析
                discard_bytes(maixcam, 1);
                continue;
            }

            vuint8 need = 4 + len; // header + cmd + len + payload(len) + checksum
            if (need > MAIXCAM_RX_BUF_SIZE)
            {
                // 不可能的情况，重置
                maixcam->data_cnt = 0;
                break;
            }

            if (maixcam->data_cnt < need)
            {
                // 不够完整帧，等待更多字节
                break;
            }

            // 计算校验和值（对 bytes[0..need-2] 求和）
            vuint8 checksum = 0;
            for (vuint8 i = 0; i < (need - 1); i++)
            {
                checksum += maixcam->rx_buf[i];
            }

            if (checksum == maixcam->rx_buf[need - 1])
            {
                data.cmd = maixcam->rx_buf[1];
                data.len = len;

                data.data = parse_payload(maixcam, data.cmd, &maixcam->rx_buf[3], data.len);

                // 移除已解析的帧
                if (maixcam->data_cnt > need)
                {
                    // 使用 discard_bytes 将已解析的部分一次性移除
                    discard_bytes(maixcam, need);
                    // 继续尝试解析下一个帧（如果有）
                    continue;
                }
                else
                {
                    // 刚好解析完所有字节
                    maixcam->data_cnt = 0;
                    break;
                }
            }
            else
            {
                // 校验失败，丢弃首字节继续解析
                discard_bytes(maixcam, 1);
                continue;
            }
        }
    }
}

maixcam_message_t maixcam_pop_data()
{
    return data;
}

void maixcam_clear(maixcam_obj_t *maixcam)
{
    data.cmd = 0;
    data.len = 0;
    data.data = 0;

    maixcam->receive_flag = 0;
}
