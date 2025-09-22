#ifndef _COMMON_H_
#define _COMMON_H_

#define CLAMP(input, max)          \
    do                             \
    {                              \
        if ((input) > (max))       \
        {                          \
            (input) = (max);       \
        }                          \
        else if ((input) < -(max)) \
        {                          \
            (input) = -(max);      \
        }                          \
    } while (0)

typedef struct
{
    float x;
    float y;
} point_2d;

typedef struct
{
    float side;
    float front;
    float yaw;
} car_pos_t;

typedef struct
{
    int bed;  // 1 或 3
    int drug; // 药品编号
} order_t;

#endif