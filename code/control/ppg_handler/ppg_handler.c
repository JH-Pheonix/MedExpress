#include "ppg_handler.h"
#include <string.h>

vuint32 ppg_ir_data[PPG_DATA_LEN] = {0};
vuint32 ppg_red_data[PPG_DATA_LEN] = {0};

static collect_state_e ppg_collect_state = WATTING_STABLE;

void ppg_collect(MAX30102_obj_t *obj)
{
    static vuint32 idx = 0;
    static vuint32 stable_cnt = 0;
    static float last_data = 0;
    static int window_ready = 0; // 标记是否已完成首次 500 样本窗口

    MAX30102_read_fifo(obj);
    switch (ppg_collect_state)
    {
    case WATTING_STABLE:
        if (stable_cnt < PPG_STABLE_CNT)
        {
            if (fabs(obj->ir - last_data) < PPG_STABLE_LIMIT)
            {
                stable_cnt++;
            }
            else
            {
                stable_cnt = 0; // 如果数据波动较大，重新计数
            }
            last_data = obj->ir;
        }
        else
        {
            // 切换到采集状态，准备采集首次窗口
            ppg_collect_state = COLLECTING_DATA;
            idx = 0;
            window_ready = 0;
        }
        break;

    case COLLECTING_DATA:
        // 先做预处理再存储
        {

            if (!window_ready)
            {
                // 首次填满 PPG_WINDOW_SIZE
                ppg_ir_data[idx++] = obj->ir;
                ppg_red_data[idx++] = obj->red;

                if (idx >= PPG_WINDOW_SIZE)
                {
                    window_ready = 1;
                    // 标记完成，等待处理（ppg_process 会在处理后继续进入滑动采集）
                    ppg_collect_state = COLLECTION_DONE;
                }
            }
            else
            {
                // 已经有一个完整窗口后，每次在开始追加新样本前检查是否需要做左移（仅在idx==PPG_WINDOW_SIZE时做一次）
                if (idx >= PPG_WINDOW_SIZE)
                {
                    // 将最近的 PPG_WINDOW_SIZE - PPG_SLIDE_STEP 个样本前移，保留最近 400 个
                    memmove(ppg_ir_data, ppg_ir_data + PPG_SLIDE_STEP, (PPG_WINDOW_SIZE - PPG_SLIDE_STEP) * sizeof(vuint32));
                    memmove(ppg_red_data, ppg_red_data + PPG_SLIDE_STEP, (PPG_WINDOW_SIZE - PPG_SLIDE_STEP) * sizeof(vuint32));
                    // 接下来从位置 PPG_WINDOW_SIZE - PPG_SLIDE_STEP 开始写入新的 PPG_SLIDE_STEP 个样本
                    idx = PPG_WINDOW_SIZE - PPG_SLIDE_STEP;
                }

                // 存入新样本
                ppg_ir_data[idx++] = obj->ir;
                ppg_red_data[idx++] = obj->red;

                // 当新的 PPG_SLIDE_STEP 个样本填满整个窗口时，标记为 COLLECTION_DONE，等待处理
                if (idx >= PPG_WINDOW_SIZE)
                {
                    ppg_collect_state = COLLECTION_DONE;
                }
            }
        }
        printf("%u\n", obj->ir);
        break;
    case COLLECTION_DONE: // 数据收集完成，等待处理。这里做一个占位
        ppg_process();
        break;
    default:
        break;
    }
}

void ppg_process()
{
    vint32 spo2, hr;
    int8_t spo2_valid, hr_valid;

    // 使用当前窗口 (PPG_WINDOW_SIZE) 的数据计算心率/血氧
    maxim_heart_rate_and_oxygen_saturation((vuint32 *)ppg_ir_data, PPG_WINDOW_SIZE, (vuint32 *)ppg_red_data, &spo2, &spo2_valid, &hr, &hr_valid);

    // 处理结果输出

    // 输出血氧值
    // printf("SpO2: %ld%%\n", spo2);
    // printf("SPO2Valid: %d\n", spo2_valid);
    // printf("Heart Rate: %d\n", hr);
    // printf("HeartRateValid: %d\n", hr_valid);

    // 处理完当前窗口后，继续进入采集下一段滑动窗口（保留最近 400，采集 100 新样本）
    ppg_collect_state = COLLECTING_DATA;
}
