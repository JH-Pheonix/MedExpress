#include "ppg.h"
#include <string.h>
#include <math.h>

float ppg_data[PPG_DATA_LEN] = {0};
static collect_state_e ppg_collect_state = WATTING_STABLE;

// Preprocessing parameters
#define HP_ALPHA 0.995f // 高通滤波系数（接近1 => 更慢的直流跟踪）
#define LP_A 0.7f       // 低通滤波系数（越大越平滑）

// 过滤器状态（静态保持）
static float hp_prev_x = 0.0f;
static float hp_prev_y = 0.0f;
static float lp_prev_y = 0.0f;

// 简单高通滤波器，用于去基线漂移（DC removal）
static float hp_filter(float x)
{
    // y[n] = alpha * (y[n-1] + x[n] - x[n-1])
    float y = HP_ALPHA * (hp_prev_y + x - hp_prev_x);
    hp_prev_x = x;
    hp_prev_y = y;
    return y;
}

// 简单一阶低通滤波器，用于平滑噪声
static float lp_filter(float x)
{
    float y = LP_A * lp_prev_y + (1.0f - LP_A) * x;
    lp_prev_y = y;
    return y;
}

// 小窗口中值抑制（3点），用于去除孤立脉冲噪声
static float median3(float a, float b, float c)
{
    // 返回三者中位数，简单且快速
    if ((a <= b && b <= c) || (c <= b && b <= a))
        return b;
    if ((b <= a && a <= c) || (c <= a && a <= b))
        return a;
    return c;
}

// 综合预处理：去基线 -> 中值抑制(3点) -> 低通平滑
// 注意：median3 需要前后点，为简化实现，此处使用最近两次原始输入的缓存
static float last_raw = 0.0f;
static float last_raw2 = 0.0f;
static float preprocess(float raw)
{
    // 中值抑制（基于当前与前两次原始值）
    float med = median3(last_raw2, last_raw, raw);
    last_raw2 = last_raw;
    last_raw = raw;

    // 去基线
    float hp = hp_filter(med);
    // 低通平滑
    float out = lp_filter(hp);
    return out;
}

void ppg_init(void)
{
    memset(ppg_data, 0, sizeof(ppg_data));
    // 初始化滤波器状态
    hp_prev_x = hp_prev_y = lp_prev_y = 0.0f;
    last_raw = last_raw2 = 0.0f;
}

void ppg_collect(MAX30102_obj_t *obj)
{
    static vuint32 idx = 0;
    static vuint32 stable_cnt = 0;
    static float last_data = 0;

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
            ppg_collect_state = COLLECTING_DATA; // 状态切换到数据收集
        }
        break;

    case COLLECTING_DATA:
        // 先做预处理再存储
        {
            float filtered = preprocess(obj->ir);
            ppg_data[idx++] = filtered;
        }

        // 防止数组越界
        if (idx >= PPG_DATA_LEN)
        {
            ppg_collect_state = COLLECTION_DONE;
        }
        break;
    case COLLECTION_DONE:
        // 处理数据逻辑
        break;
    default:
        break;
    }
}