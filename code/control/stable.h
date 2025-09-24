#ifndef _CONTROL_STABLE_H_
#define _CONTROL_STABLE_H_

#include "zf_common_typedef.h"
#include "attitude.h"
#include "init.h"

// 匀速控制
void stable_roting(float target_angle, vuint16 duty);
void stable_moving();

#endif