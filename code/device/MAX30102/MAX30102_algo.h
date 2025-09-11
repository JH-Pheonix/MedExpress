// 简单心率估计算法与温度传感器模板（TMP117）
#ifndef _DEVICE_MAX30102_ALGO_H_
#define _DEVICE_MAX30102_ALGO_H_

#include "MAX30102.h"
#include "zf_common_typedef.h"

// 心率估计器（基于 IR 波形）
typedef struct
{
    vuint16 sample_rate;   // 采样率 Hz
    float dc;              // IIR DC 分量
    float filt;            // 带通后信号（简化实现）
    float prev;            // 上一滤波值
    float threshold;       // 峰值检测阈值
    vuint32 sample_idx;    // 采样计数
    vuint32 last_peak_idx; // 上次峰位置（样本索引）
    vuint32 intervals[8];  // 最近峰间隔（样本数）
    vuint8 interval_cnt;   // 记录数

    // 下面为增强字段
    float lp1;            // 带通 IIR 低通状态
    float hp1;            // 带通 IIR 高通状态
    float bpm_history[4]; // 平滑 BPM 缓存
    vuint8 bpm_hist_cnt;
    vuint16 last_bpm; // 最后输出的 BPM
    // ring buffer 用于估算 AC 峰峰值（短窗口）
    float recent_buf[32];
    vuint8 recent_idx;
    // 最小峰间隔（样本数），防抖
    vuint16 min_interval_samples;
    // AC/DC 比阈值，用于 SQI（如 ac_pp / dc >= ac_threshold_ratio 才接受）
    float ac_threshold_ratio;
    // 信号质量 0=差 1=好
    vuint8 quality;
} MAX30102_HR_t;

// 初始化心率估计器
void MAX30102_hr_init(MAX30102_HR_t *hr, vuint16 sample_rate);

// 喂入一条 IR 样本（原始 18-bit 值），返回 0 表示无新心率，否则返回估算的 BPM（整数）
vuint16 MAX30102_hr_feed(MAX30102_HR_t *hr, vuint32 ir_sample);

// 重置估计器状态
void MAX30102_hr_reset(MAX30102_HR_t *hr);

// 获取平滑后的 BPM（若无有效 BPM 返回 0）
vuint16 MAX30102_hr_get_smoothed(MAX30102_HR_t *hr);

// 设置参数：最大可接受 BPM 与 AC/DC 阈值（例 0.02）
void MAX30102_hr_set_params(MAX30102_HR_t *hr, vuint16 max_bpm, float ac_threshold_ratio);

// 获取当前信号质量（0/1）
vuint8 MAX30102_hr_get_quality(MAX30102_HR_t *hr);

// ---------------- SpO2 模块（简化实现） ----------------
typedef struct
{
    vuint16 sample_rate; // 采样率
    // 简单滑动窗口用于估算 DC（低频）
    float dc_red;
    float dc_ir;
    // 用于估算 AC（通过减去 DC 取绝对值后移动平均）
    float ac_red_avg;
    float ac_ir_avg;
    vuint16 window_size; // 计算窗口大小（样本数）
    vuint16 sample_count;
    float last_r;
    vuint8 last_spo2;
    // recent buffers 用于短窗口峰峰值估算（用于 SQI）
    float recent_red_buf[32];
    float recent_ir_buf[32];
    vuint8 recent_idx;
    // 峰峰值估算
    float ac_pp_red;
    float ac_pp_ir;
    // PP / DC 的阈值比率（用于判别信号质量）
    float ac_pp_threshold_ratio;
    // 质量标记：0 = 差，1 = 好
    vuint8 quality;
} MAX30102_SPO2_t;

// 初始化 SpO2 模块，window_size 建议为采样率 * 2~4 秒
void MAX30102_spo2_init(MAX30102_SPO2_t *s, vuint16 sample_rate, vuint16 window_size);

// 喂入一对样本（red, ir），返回估算的 SpO2 值（0 表示无效）
vuint8 MAX30102_spo2_feed(MAX30102_SPO2_t *s, vuint32 red_sample, vuint32 ir_sample);

// 获取最近一次估算的 R 与 SpO2（若需要）
float MAX30102_spo2_get_last_r(MAX30102_SPO2_t *s);
vuint8 MAX30102_spo2_get_last_spo2(MAX30102_SPO2_t *s);

// 获取 SpO2 信号质量（0/1）
vuint8 MAX30102_spo2_get_quality(MAX30102_SPO2_t *s);

#endif
