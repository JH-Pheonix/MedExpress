#ifndef _SYSTEM_INIT_H_
#define _SYSTEM_INIT_H_

#include "imu.h"
#include "motor.h"
#include "key.h"
#include "lcd.h"
#include "Attitude.h"

void system_init(void);

extern motor_obj_t motor1;
extern motor_obj_t motor2;
extern motor_obj_t motor3;
extern motor_obj_t motor4;

#endif