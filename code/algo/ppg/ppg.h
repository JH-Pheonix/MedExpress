#ifndef _ALGO_PPG_H_
#define _ALGO_PPG_H_

#include "zf_common_typedef.h"
#include "MAX30102.h"

#define PPG_DATA_THRESHOLD 1000000
#define PPG_DATA_LEN 6000

#define PPG_STABLE_CNT 100     // 采集稳定时间
#define PPG_STABLE_LIMIT 10000 // 稳定范围

typedef enum
{
    WATTING_STABLE,
    COLLECTING_DATA,
    COLLECTION_DONE
} collect_state_e;

#endif