#ifndef _SYSTEM_STATEMACHINE_H_
#define _SYSTEM_STATEMACHINE_H_

#include "zf_common_typedef.h"
#include "key.h"
#include "common.h"
#include "stp23l.h"
#include "init.h"
#include "attitude.h"

typedef enum
{
    EVENT_WAITTING_POS_ONE,   // 一号床机器姿态记录
    EVENT_WAITTING_POS_THREE, // 三号床机器姿态记录
    EVENT_NURSE,
    EVENT_WATTING_QRCODE,
    EVENT_GO_TO_BED,
    EVENT_BED_ONE,
    EVENT_BED_THREE,
    EVENT_BACK_HOME,
    EVENT_STOP
} state_machine_event_e;

void state_machine_execute();

#endif