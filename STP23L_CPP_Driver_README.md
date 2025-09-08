# STP-23L 激光雷达 C++ 驱动程序

## 概述

这是为 LDROBOT STP-23L 激光雷达开发的 C++ 驱动程序，适用于 TC377 开源库平台。该驱动程序提供了完整的面向对象接口，同时保持与C代码的兼容性。

## 主要特性

- **面向对象设计**: 使用C++类封装，提供清晰的接口
- **C兼容性**: 保留C风格接口，可与现有C代码无缝集成
- **RAII支持**: 自动资源管理，减少内存泄漏风险
- **类型安全**: 使用C++强类型系统，减少运行时错误
- **现代C++特性**: 支持移动语义、auto关键字等

## 文件说明

- `stp23l.h` - C++头文件，包含类定义和C兼容接口
- `stp23l.cpp` - C++实现文件，包含类方法实现和C兼容函数
- `stp23l_cpp_example.cpp` - C++使用示例

## 硬件连接

| 激光雷达引脚 | 单片机引脚 | 说明 |
|-------------|-----------|------|
| VCC | 5V | 电源正极 |
| GND | GND | 电源地 |
| TX | UART2_RX_P10_6 | 雷达发送，单片机接收 |
| RX | UART2_TX_P10_5 | 雷达接收，单片机发送 |

## C++ 类接口

### 基本使用

```cpp
#include "stp23l.h"

// 创建激光雷达实例
STP23L lidar;

// 初始化
if (lidar.init()) {
    // 开始扫描
    if (lidar.start_scan()) {
        // 主循环
        while (true) {
            lidar.handler();
            
            if (lidar.is_data_ready()) {
                stp23l_frame_t frame;
                if (lidar.get_scan_data(frame)) {
                    // 处理扫描数据
                    process_data(frame);
                }
            }
        }
    }
}
```

### 主要方法

#### 初始化和控制
```cpp
bool init();                        // 初始化激光雷达
bool start_scan();                  // 开始扫描
bool stop_scan();                   // 停止扫描
bool restart_device();              // 重启设备
void handler();                     // 数据处理（需要在主循环中调用）
```

#### 数据获取
```cpp
bool get_scan_data(stp23l_frame_t &frame);           // 获取扫描数据
bool get_device_info();                              // 获取设备信息
bool get_health_status();                            // 获取健康状态
```

#### 状态查询
```cpp
stp23l_status_enum get_status() const;               // 获取设备状态
bool is_data_ready() const;                          // 检查数据是否就绪
const stp23l_frame_t& get_current_frame() const;     // 获取当前帧数据
const stp23l_device_info_t& get_device_info_struct() const;  // 获取设备信息结构体
const stp23l_health_t& get_health_info() const;      // 获取健康信息
```

#### 静态工具方法
```cpp
static float convert_angle(uint16 raw_angle);        // 角度转换
static bool is_distance_valid(uint16 distance);      // 距离有效性检查
static bool is_intensity_valid(uint16 intensity);    // 强度有效性检查
static void uart_callback();                         // 串口回调（静态）
```

## C 兼容接口

如果需要在C代码中使用，可以使用以下接口：

```c
#include "stp23l.h"

// 初始化
uint8 result = stp23l_init();

// 开始扫描
stp23l_start_scan();

// 主循环
while (1) {
    stp23l_handler();
    
    stp23l_frame_t frame;
    if (stp23l_get_scan_data(&frame) == 0) {
        // 处理数据
    }
}
```

## 高级特性

### RAII 资源管理

```cpp
class LidarManager {
private:
    STP23L& lidar_ref;
    bool auto_stop;

public:
    LidarManager(STP23L& lidar, bool start_scan = true) 
        : lidar_ref(lidar), auto_stop(start_scan) {
        if (start_scan) {
            lidar_ref.start_scan();
        }
    }
    
    ~LidarManager() {
        if (auto_stop) {
            lidar_ref.stop_scan();
        }
    }
};

// 使用示例
void scan_for_a_while() {
    LidarManager manager(lidar);  // 自动开始扫描
    
    // 进行扫描工作
    for (int i = 0; i < 1000; i++) {
        lidar.handler();
        // ...
    }
    
    // 离开作用域时自动停止扫描
}
```

### 现代C++风格的数据处理

```cpp
void process_lidar_data_modern(const stp23l_frame_t& frame) {
    // 使用auto关键字
    const auto& points = frame.points;
    
    // 使用范围for循环
    for (const auto& point : points) {
        if (STP23L::is_distance_valid(point.distance)) {
            // 处理有效点
            printf("Valid point: %.1f° %dmm\n", point.angle, point.distance);
        }
    }
    
    // 使用lambda表达式进行筛选
    auto is_close_obstacle = [](const stp23l_point_t& p) {
        return STP23L::is_distance_valid(p.distance) && p.distance < 500;
    };
    
    // 检查是否有近距离障碍物
    for (const auto& point : points) {
        if (is_close_obstacle(point)) {
            printf("Close obstacle detected!\n");
            break;
        }
    }
}
```

## 配置参数

可以在 `stp23l.h` 中修改以下配置：

```cpp
#define STP23L_UART             (UART_2)        // 使用的串口
#define STP23L_TX_PIN           (UART2_TX_P10_5) // TX引脚
#define STP23L_RX_PIN           (UART2_RX_P10_6) // RX引脚
#define STP23L_BAUDRATE         (230400)        // 波特率
```

## 中断配置

在 `isr.c` 文件中添加串口中断处理：

```cpp
// C++方式
extern "C" void uart2_rx_interrupt(void) {
    STP23L::uart_callback();
}

// 或者C方式
void uart2_rx_interrupt(void) {
    stp23l_uart_callback();
}
```

## 编译注意事项

1. **编译器**: 确保使用支持C++11或更高版本的编译器
2. **文件扩展名**: 确保将实现文件命名为 `.cpp`
3. **链接**: 确保项目配置支持C++编译

## 使用步骤

### 1. 添加文件到工程
- `code/device/stp23l/stp23l.h`
- `code/device/stp23l/stp23l.cpp`

### 2. C++项目中使用
```cpp
#include "stp23l.h"

STP23L lidar;

int main() {
    if (lidar.init()) {
        lidar.start_scan();
        // 主循环...
    }
    return 0;
}
```

### 3. C项目中使用
```c
#include "stp23l.h"

int main() {
    if (stp23l_init() == 0) {
        stp23l_start_scan();
        // 主循环...
    }
    return 0;
}
```

## 性能优化

1. **内联函数**: 工具函数使用内联实现，减少函数调用开销
2. **引用传递**: 大型结构体使用引用传递，避免不必要的拷贝
3. **静态方法**: 工具函数实现为静态方法，无需实例化
4. **编译时优化**: 使用const和constexpr提高编译时优化

## 故障排除

1. **编译错误**: 检查C++编译器版本和项目配置
2. **链接错误**: 确保正确包含所有必要的文件
3. **运行时错误**: 使用C++异常处理机制进行调试

## 版本历史

- v1.0 (2025-01-01): C版本初始版本
- v2.0 (2025-09-04): C++版本，增加面向对象接口

## 作者

ZeroHzzzz

## 许可证

本驱动程序遵循 GPL3.0 开源许可证，与 TC377 开源库保持一致。
