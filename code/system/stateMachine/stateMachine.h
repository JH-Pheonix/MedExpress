#ifndef _SYSTEM_STATEMACHINE_H_
#define _SYSTEM_STATEMACHINE_H_

#include "zf_common_typedef.h"
#include "common.h"

typedef struct
{
    vuint8 sig;
} event;

typedef struct
{
    vuint8 state;
    float target_yaw;
    vuint64 barcode;
    point_2d target_point[5];  // 目标点位(惯导)
    uint32 target_point_order; // 12345
    vuint8 curr_point;
    point_2d waypoint[5][5][10]; // 路点(i->j, k)
    point_2d finetune_pos[2];    // 微调目标位置
} state_machine;

void state_machine_init(void);
void state_machine_run(void);

#endif