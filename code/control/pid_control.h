#ifndef _CONTROL_PID_CONTROL_H_
#define _CONTROL_PID_CONTROL_H_

#include "zf_common_typedef.h"

void control_init(void);
void control_handler(void);
float control_angle_pid(void);

extern float diff;
#endif