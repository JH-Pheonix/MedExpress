#ifndef PPG_HANDLER_H
#define PPG_HANDLER_H

#include "zf_common_typedef.h"
#include "MAX30102.h"
#include "init.h"
#include "ppg.h"

#define PPG_DATA_THRESHOLD 1000000
#define PPG_DATA_LEN 6000

#define PPG_STABLE_CNT 100     // 采集稳定时间
#define PPG_STABLE_LIMIT 10000 // 稳定范围

#define PPG_WINDOW_SIZE 500
#define PPG_SLIDE_STEP 100

typedef enum
{
    WATTING_STABLE,
    COLLECTING_DATA,
    COLLECTION_DONE
} collect_state_e;

void ppg_collect(MAX30102_obj_t *obj);
void ppg_process();

#endif