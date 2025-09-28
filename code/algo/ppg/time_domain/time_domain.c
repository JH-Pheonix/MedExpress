#include "time_domain.h"
#include <math.h>

int td_count_peaks(const float *data, size_t len, size_t min_distance, float threshold)
{
    if (!data || len == 0)
    {
        return 0;
    }

    if (min_distance == 0)
    {
        min_distance = 1;
    }

    if (len < 3)
    {
        // 不足以形成峰（需要前后各一个点作为比较）
        return 0;
    }

    // 计算最大值和均值，用于自适应阈值
    float maxv = -INFINITY;
    double sum = 0.0;
    for (size_t i = 0; i < len; ++i)
    {
        float v = data[i];
        if (isnan(v))
            continue;
        sum += v;
        if (v > maxv)
            maxv = v;
    }
    double mean = sum / (double)len;

    if (!(threshold > 0.0f))
    {
        // 自适应阈值：均值 + 50% * (最大 - 均值)
        threshold = (float)(mean + 0.5 * (maxv - mean));
    }

    int count = 0;
    long last_idx = -((long)min_distance) - 1; // 上次被接受的峰索引
    float last_val = 0.0f;

    // 从第二个样点到倒数第二个样点检测局部最大
    for (size_t i = 1; i + 1 < len; ++i)
    {
        float v = data[i];
        if (isnan(v))
            continue;

        // 简单局部极大值判定：严格大于左右邻点，并且高于阈值
        if (v > data[i - 1] && v > data[i + 1] && v > threshold)
        {
            if (count == 0)
            {
                // 第一个峰
                count = 1;
                last_idx = (long)i;
                last_val = v;
            }
            else
            {
                long dist = (long)i - last_idx;
                if ((size_t)dist < min_distance)
                {
                    // 距离过近，保留更高的一个
                    if (v > last_val)
                    {
                        last_idx = (long)i;
                        last_val = v;
                    }
                }
                else
                {
                    // 足够远，接受为新峰
                    count++;
                    last_idx = (long)i;
                    last_val = v;
                }
            }
        }
    }

    return count;
}
