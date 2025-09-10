#ifndef _DEVICE_MOTOR_H_
#define _DEVICE_MOTOR_H_

#include "zf_driver_pwm.h"
#include "zf_common_typedef.h"
#include "zf_driver_gpio.h"

typedef struct
{
    pwm_channel_enum pwm_pin;
    gpio_pin_enum dir_pin;
    vuint32 freq;
    vuint8 status;
    vuint32 deadzone;
} motor_obj_t;

motor_obj_t motor_init(pwm_channel_enum pwm_pin, gpio_pin_enum dir_pin, vuint32 freq, vuint32 deadzone);
void motor_set_pwm(motor_obj_t *motor, vint32 duty);

#endif