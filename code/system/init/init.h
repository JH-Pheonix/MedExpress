#ifndef _SYSTEM_INIT_H_
#define _SYSTEM_INIT_H_

#include "imu.h"
#include "motor.h"
#include "key.h"
#include "lcd.h"
#include "Attitude.h"
#include "stp23l.h"
#include "maixcam.h"
#include "asrpro.h"

void system_init(void);

extern motor_obj_t motor1;
extern motor_obj_t motor2;
extern motor_obj_t motor3;
extern motor_obj_t motor4;

extern stp23l_obj_t lidar1;
extern stp23l_obj_t lidar2;

extern maixcam_obj_t maixcam1;

extern asrpro_obj_t asrpro1;

#endif