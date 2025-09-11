#include "MAX30102_algo.h"

// 心率算法实现（升级版）
void MAX30102_hr_init(MAX30102_HR_t *hr, vuint16 sample_rate)
{
    hr->sample_rate = sample_rate;
    hr->dc = 0.0f;
    hr->filt = 0.0f;
    hr->prev = 0.0f;
    hr->threshold = 0.0f;
    hr->sample_idx = 0;
    hr->last_peak_idx = 0;
    hr->interval_cnt = 0;
    hr->lp1 = 0.0f;
    hr->hp1 = 0.0f;
    for (vuint8 i = 0; i < 4; i++)
        hr->bpm_history[i] = 0.0f;
    hr->bpm_hist_cnt = 0;
    hr->last_bpm = 0;
    for (vuint8 i = 0; i < 32; i++)
        hr->recent_buf[i] = 0.0f;
    hr->recent_idx = 0;
    hr->min_interval_samples = (vuint16)(sample_rate * 0.4f); // 最少 0.4 秒间隔
    hr->ac_threshold_ratio = 0.02f;                           // 默认 AC/DC 最小比
    hr->quality = 0;
}

void MAX30102_hr_set_params(MAX30102_HR_t *hr, vuint16 max_bpm, float ac_threshold_ratio)
{
    (void)max_bpm; // 当前实现未使用 max_bpm
    hr->ac_threshold_ratio = ac_threshold_ratio;
}

vuint8 MAX30102_hr_get_quality(MAX30102_HR_t *hr)
{
    return hr->quality;
}

static float compute_pp_from_recent(MAX30102_HR_t *hr)
{
    // 计算 recent_buf 的峰峰值
    float vmin = hr->recent_buf[0];
    float vmax = vmin;
    for (vuint8 i = 1; i < 32; i++)
    {
        float v = hr->recent_buf[i];
        if (v < vmin)
            vmin = v;
        if (v > vmax)
            vmax = v;
    }
    return vmax - vmin;
}

vuint16 MAX30102_hr_feed(MAX30102_HR_t *hr, vuint32 ir_sample)
{
    float x = (float)ir_sample;

    // 更新 recent buffer
    hr->recent_buf[hr->recent_idx++] = x;
    if (hr->recent_idx >= 32)
        hr->recent_idx = 0;

    // DC 去除
    const float alpha = 0.01f;
    hr->dc += alpha * (x - hr->dc);

    // 带通近似
    const float a_lp = 0.2f;
    hr->lp1 += a_lp * (x - hr->lp1);
    hr->hp1 = x - hr->lp1;
    const float beta = 0.25f;
    hr->filt = beta * hr->hp1 + (1.0f - beta) * hr->filt;

    // 自适应阈值
    float absf = hr->filt > 0 ? hr->filt : -hr->filt;
    hr->threshold = 0.7f * hr->threshold + 0.3f * absf;

    // 计算当前 recent 窗口峰峰值并与 DC 比较以判断质量
    float pp = compute_pp_from_recent(hr);
    float ac_dc_ratio = (hr->dc > 1e-6f) ? (pp / hr->dc) : 0.0f;
    hr->quality = (ac_dc_ratio >= hr->ac_threshold_ratio) ? 1 : 0;

    vuint16 bpm = 0;

    // 峰值检测带最小间隔去抖
    if (hr->filt > hr->prev && hr->filt > hr->threshold)
    {
        vuint32 idx = hr->sample_idx;
        vuint32 interval = idx - hr->last_peak_idx;
        if (hr->last_peak_idx != 0 && interval >= hr->min_interval_samples)
        {
            // 存储间隔
            hr->intervals[hr->interval_cnt % 8] = interval;
            hr->interval_cnt++;

            // 使用修剪平均（去掉最大/最小）计算平均间隔
            vuint8 cnt = hr->interval_cnt < 8 ? hr->interval_cnt : 8;
            // 复制到临时数组
            vuint32 tmp[8];
            for (vuint8 i = 0; i < cnt; i++)
                tmp[i] = hr->intervals[i];
            // 排序（简单选择排序，cnt <= 8）
            for (vuint8 i = 0; i < cnt; i++)
            {
                vuint8 min_i = i;
                for (vuint8 j = i + 1; j < cnt; j++)
                    if (tmp[j] < tmp[min_i])
                        min_i = j;
                vuint32 t = tmp[i];
                tmp[i] = tmp[min_i];
                tmp[min_i] = t;
            }
            // 修剪平均
            vuint32 sum = 0;
            vuint8 start = 0, end = cnt;
            if (cnt > 2)
            {
                start = 1;
                end = cnt - 1;
            }
            vuint8 use_cnt = end - start;
            for (vuint8 i = start; i < end; i++)
                sum += tmp[i];
            if (use_cnt > 0)
            {
                float avg_interval = (float)sum / (float)use_cnt;
                float raw_bpm = (60.0f * (float)hr->sample_rate) / avg_interval;
                if (raw_bpm > 30.0f && raw_bpm < 220.0f && hr->quality)
                {
                    // 平滑历史
                    hr->bpm_history[hr->bpm_hist_cnt % 4] = raw_bpm;
                    hr->bpm_hist_cnt++;
                    vuint8 cnt2 = hr->bpm_hist_cnt < 4 ? hr->bpm_hist_cnt : 4;
                    float s = 0.0f;
                    for (vuint8 i = 0; i < cnt2; i++)
                        s += hr->bpm_history[i];
                    bpm = (vuint16)(s / (float)cnt2 + 0.5f);
                    hr->last_bpm = bpm;
                }
            }
        }

        hr->last_peak_idx = idx;
    }

    hr->prev = hr->filt;
    hr->sample_idx++;

    return bpm;
}

// 重置估计器
void MAX30102_hr_reset(MAX30102_HR_t *hr)
{
    MAX30102_hr_init(hr, hr->sample_rate);
}

// 获取最近平滑 BPM（如果没有则返回 0）
vuint16 MAX30102_hr_get_smoothed(MAX30102_HR_t *hr)
{
    return hr->last_bpm;
}

// ---------------- SpO2 模块实现（简化） ----------------
void MAX30102_spo2_init(MAX30102_SPO2_t *s, vuint16 sample_rate, vuint16 window_size)
{
    s->sample_rate = sample_rate;
    s->dc_red = 0.0f;
    s->dc_ir = 0.0f;
    s->ac_red_avg = 0.0f;
    s->ac_ir_avg = 0.0f;
    s->window_size = window_size;
    s->sample_count = 0;
    s->last_r = 0.0f;
    s->last_spo2 = 0;
    for (vuint8 i = 0; i < 32; i++)
    {
        s->recent_red_buf[i] = 0.0f;
        s->recent_ir_buf[i] = 0.0f;
    }
    s->recent_idx = 0;
    s->ac_pp_red = 0.0f;
    s->ac_pp_ir = 0.0f;
    s->ac_pp_threshold_ratio = 0.02f;
    s->quality = 0;
}

vuint8 MAX30102_spo2_get_quality(MAX30102_SPO2_t *s)
{
    return s->quality;
}

vuint8 MAX30102_spo2_feed(MAX30102_SPO2_t *s, vuint32 red_sample, vuint32 ir_sample)
{
    // 使用 IIR 更新 DC
    const float alpha = 0.01f;
    float redf = (float)red_sample;
    float irf = (float)ir_sample;
    s->dc_red += alpha * (redf - s->dc_red);
    s->dc_ir += alpha * (irf - s->dc_ir);

    // 更新 recent buffers
    s->recent_red_buf[s->recent_idx] = redf;
    s->recent_ir_buf[s->recent_idx] = irf;
    s->recent_idx++;
    if (s->recent_idx >= 32)
        s->recent_idx = 0;

    // AC 估算：绝对差的 IIR 平滑
    float ac_red = redf - s->dc_red;
    float ac_ir = irf - s->dc_ir;
    float abs_red = ac_red > 0 ? ac_red : -ac_red;
    float abs_ir = ac_ir > 0 ? ac_ir : -ac_ir;
    const float beta = 0.05f;
    s->ac_red_avg = beta * abs_red + (1.0f - beta) * s->ac_red_avg;
    s->ac_ir_avg = beta * abs_ir + (1.0f - beta) * s->ac_ir_avg;

    // 计算 PP
    float rmin = s->recent_red_buf[0], rmax = rmin;
    float imax = s->recent_ir_buf[0], imin = imax;
    for (vuint8 i = 1; i < 32; i++)
    {
        float rv = s->recent_red_buf[i];
        if (rv < rmin)
            rmin = rv;
        if (rv > rmax)
            rmax = rv;
        float iv = s->recent_ir_buf[i];
        if (iv < imin)
            imin = iv;
        if (iv > imax)
            imax = iv;
    }
    s->ac_pp_red = rmax - rmin;
    s->ac_pp_ir = imax - imin;

    // 判断质量
    float ratio_red = (s->dc_red > 1e-6f) ? (s->ac_pp_red / s->dc_red) : 0.0f;
    float ratio_ir = (s->dc_ir > 1e-6f) ? (s->ac_pp_ir / s->dc_ir) : 0.0f;
    s->quality = (ratio_red >= s->ac_pp_threshold_ratio && ratio_ir >= s->ac_pp_threshold_ratio) ? 1 : 0;

    s->sample_count++;
    // 只有在足够样本后才计算
    if (s->sample_count < s->window_size)
        return 0;

    // 计算比值 R，安全检查
    if (s->dc_ir <= 1e-6f || s->ac_ir_avg <= 1e-6f)
        return 0;
    float ratio = (s->ac_red_avg / s->dc_red) / (s->ac_ir_avg / s->dc_ir);
    s->last_r = ratio;

    // 仅在质量良好时才返回 SpO2
    if (!s->quality)
        return 0;

    // 线性映射到 SpO2（示例系数，需校准）
    float spo2f = 110.0f - 25.0f * ratio;
    if (spo2f > 100.0f)
        spo2f = 100.0f;
    if (spo2f < 50.0f)
        spo2f = 0.0f; // 低于 50 认为无效

    s->last_spo2 = (vuint8)(spo2f + 0.5f);
    return s->last_spo2;
}

float MAX30102_spo2_get_last_r(MAX30102_SPO2_t *s)
{
    return s->last_r;
}

vuint8 MAX30102_spo2_get_last_spo2(MAX30102_SPO2_t *s)
{
    return s->last_spo2;
}