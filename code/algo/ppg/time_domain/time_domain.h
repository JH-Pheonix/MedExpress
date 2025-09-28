#ifndef _ALGO_PPG_TIME_DOMAIN_H_
#define _ALGO_PPG_TIME_DOMAIN_H_

#include "zf_common_typedef.h"
#include <stddef.h>

// 计算信号中的主要波峰数量（时域法）
// data: 指向输入信号数组（浮点），len: 数组长度
// min_distance: 两个相邻波峰之间的最小样点间隔（样点数），如果为0则默认为1
// threshold: 峰值判定阈值；如果传入小于等于0的值，函数将自动根据最大/均值估算阈值
// 返回值：检测到的主要峰数量
int td_count_peaks(const float *data, size_t len, size_t min_distance, float threshold);

#endif
