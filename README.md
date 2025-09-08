# device 驱动说明

本 README 汇总 `code/device` 目录下常用外设驱动（IMU / Key / LCD / STP23L）的快速说明与使用示例，便于在工程中直接调用。

目录

-   `imu/` — IMU 抽象层与具体驱动（imu660ra/imu660rb/imu963ra/adis16505）
-   `key/` — 按键处理（轮询/中断/消息队列）
-   `lcd/` — TFT 显示封装（基于 tft180）
-   `stp23l/` — STP23L 激光雷达协议解析与发送

使用约定（契约）

-   头文件：在使用驱动前包含相应头文件，如 `#include "imu.h"`。
-   初始化：多数驱动提供 `*_init()` 函数，请在主函数或系统初始化阶段调用。
-   错误/返回：部分底层驱动返回 0 表示成功、非 0 表示失败（例如 ADIS 的 `adis_init()`）；但上层包装（如 `imu_init()`）内部仅打印错误提示，调用方可根据需要再做检测。

常见边界/注意点

-   确保 `pin.h` 中的引脚与 SPI/UART 配置正确。
-   串口/中断环境：`stp23l` 依赖串口中断或轮询将字节喂给 `stp23l_feed_byte()`，建议在 UART RX 回调内调用 `stp23l_receiver_callback()`。
-   线程/中断安全：驱动内部使用环形队列，消费（pop）和产生（push）在主循环或中断中都可能发生。若在多线程/RTOS 下使用，请注意并发保护。

示例代码（使用片段可直接复制到 `main.c`）

## 1. IMU

接口（摘自 `imu.h`）

-   `void imu_init(imu_device_enum device);`
-   `imu_data_t imu_get_data(void);` // 返回 accel/gyro

示例：使用 ADIS16505（SPI）读数

```c
#include "imu.h"

int main(void)
{
	// 初始化外设（SPI/GPIO）通常在 pin.h / board init 中完成
	imu_init(IMU_DEVICE_ADIS16505);

	while (1)
	{
		imu_data_t d = imu_get_data();
		// 使用 d.accel_x / d.accel_y / d.accel_z / d.gyro_x ...
		// 例如打印或发送到上位机
		printf("ACC: %.3f %.3f %.3f  GYRO: %.3f %.3f %.3f\n",
			   d.accel_x, d.accel_y, d.accel_z,
			   d.gyro_x, d.gyro_y, d.gyro_z);
		system_delay_ms(100);
	}
}
```

说明：若使用 `imu_init(IMU_DEVICE_660RA)` 等，底层会调用对应的 `imu660ra_init()` 等初始化函数。

## 2. Key（按键）

接口（摘自 `key.h`）

-   `void key_init_rewrite(KEY_e key);` // 初始化某个按键或全部
-   `KEY_e key_scan(void);` // 轮询扫描，返回按键编号
-   `vuint8 key_get_msg(KEY_MSG_t *keymsg);` // 从内部消息队列取消息，返回 1 表示有消息

示例：轮询或消息读取

```c
#include "key.h"

int main(void)
{
	key_init_rewrite(KEY_NONE); // 初始化所有定义的按键

	while (1)
	{
		// 方式一：直接轮询
		KEY_e k = key_scan();
		if (k != KEY_NONE)
		{
			printf("Key pressed: %d\n", (int)k);
		}

		// 方式二：从消息队列读取（推荐在主循环处理）
		KEY_MSG_t msg;
		while (key_get_msg(&msg))
		{
			// msg.key / msg.status
			printf("Key msg: key=%d status=%d\n", msg.key, msg.status);
		}

		system_delay_ms(10);
	}
}
```

## 3. LCD

接口（摘自 `lcd.h`）

-   `void lcd_init(void);`
-   `void lcd_clear(void);`
-   `void lcd_show_string(vuint16 x, vuint16 y, const char *dat);`
-   以及带颜色/数值/浮点的 `lcd_show_*` 系列函数

示例：初始化并显示文本

```c
#include "lcd.h"

int main(void)
{
	lcd_init();
	lcd_clear();
	lcd_show_string(0, 0, "Hello STP-23L");
	lcd_show_string_color(0, 1, "IMU OK", RGB565_GREEN, RGB565_BLACK);

	while (1)
	{
		// 更新屏幕数据
		system_delay_ms(1000);
	}
}
```

注意：坐标单位为字符网格（x _ CHAR_WIDTH, y _ CHAR_HEIGHT）。颜色常量在 `zf_device_tft180.h` 中定义。

## 4. STP23L（激光雷达）

接口（摘自 `stp23l.h`）

-   `void stp23l_init(void);`
-   `int stp23l_get_start(uart_index_enum uartn);` // 发送开始采样命令
-   `int stp23l_send_stop_cmd(uart_index_enum uartn);`
-   `int stp23l_send_reset_cmd(uart_index_enum uartn);`
-   `void stp23l_receiver_callback(uart_index_enum uartn);` // 串口中断/轮询调用
-   `int stp23l_pop_frame(stp23l_frame_t *out);` // 从环形队列取一帧

使用示例（主循环 + UART RX 回调）

```c
#include "stp23l.h"

// 在系统初始化阶段
int main(void)
{
	stp23l_init();
	// 向设备下发开始测距命令
	stp23l_get_start(STP23L_UART);

	while (1)
	{
		stp23l_frame_t frame;
		if (stp23l_pop_frame(&frame))
		{
			// 处理一帧数据
			for (int i = 0; i < LIDAR_POINT_NUM; ++i)
			{
				printf("Pt %d dist=%d conf=%u\n", i, frame.points[i].distance, frame.points[i].confidence);
			}
		}
		system_delay_ms(10);
	}
}

IFX_INTERRUPT(uart1_rx_isr, UART1_INT_VECTAB_NUM, UART1_RX_INT_PRIO)
{
    interrupt_global_enable(0); // 开启中断嵌套
    stp23l_receiver_callback(uartn);
}
```

说明：`stp23l` 实现了协议解析并把有效的 frame/ack 推入内部队列，用户通过 `stp23l_pop_frame()` / `stp23l_pop_ack()` 拉取并处理。

## 调试与排错建议

-   若设备无法工作，先检查 `pin.h` 中的管脚定义是否与硬件一致。
-   使用串口/逻辑分析仪查看 STP23L 通信，以确认包格式与校验。
-   对于 IMU（SPI）类设备，确认 SPI 模式、时钟与 CS 控制是否正确。
