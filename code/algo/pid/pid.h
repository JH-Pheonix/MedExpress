#ifndef _ALGO_PID_H_
#define _ALGO_PID_H_

#include "zf_common_typedef.h"

typedef struct
{
    // PID 三参数
    float32 Kp;
    float32 Ki;
    float32 Kd;

    float32 max_out;  // 最大输出
    float32 max_iout; // 最大积分输出

    float32 set;
    float32 fdb;

    float32 out;
    float32 Pout;
    float32 Iout;
    float32 Dout;
    float32 Dbuf[3];  // 微分项 0最新 1上一次 2上上次
    float32 error[3]; // 误差项 0最新 1上一次 2上上次

} pid_type_def;

#endif