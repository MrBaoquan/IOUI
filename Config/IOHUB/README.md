# IOHUB 配置指南

## 目录

1. [概述](#概述)
2. [协议配置](#协议配置)
3. [帧格式配置](#帧格式配置)
4. [输入输出映射](#输入输出映射)
5. [支持的协议](#支持的协议)
6. [配置示例](#配置示例)
7. [高级特性](#高级特性)

---

## 概述

IOHUB 是一个统一的 IO 设备管理框架，支持多种通信协议和数据格式。主要特性：

- ✅ **多协议支持**：UDP、TCP Client、TCP Server、Serial
- ✅ **灵活的数据格式**：ASCII、HEX、帧协议
- ✅ **自动帧解析**：支持自定义帧头、帧尾、通道偏移
- ✅ **值编码映射**：支持输入输出不同的值编码（如淘达电子协议）
- ✅ **通道映射**：显式映射 + 自动解析的混合模式
- ✅ **输入保持机制**：可配置的自动清零或永久保持
- ✅ **多设备管理**：同时管理多个不同协议的设备

---

## 协议配置

### URI 格式

IOHUB 使用 URI 格式配置协议：

```ini
protocol=<协议类型>://<地址>:<端口>?<参数1>=<值1>&<参数2>=<值2>
```

### 支持的协议类型

#### 1. TCP 客户端

```ini
protocol=tcp://192.168.1.100:8080?reconnect=3000&keepalive=true
```

**参数说明：**
- `reconnect`: 重连间隔（毫秒），默认 3000
- `keepalive`: 保持连接，默认 true

**使用场景：** 连接到远程服务器

#### 2. TCP 服务器

```ini
protocol=tcp-server://0.0.0.0:8080?keepalive=true
```

**参数说明：**
- `0.0.0.0`: 监听所有网卡
- `keepalive`: 保持连接，默认 true

**使用场景：** 作为服务器接受客户端连接

#### 3. UDP

```ini
protocol=udp://192.168.1.100:9000?localport=5000
```

**参数说明：**
- `localport`: 本地端口，默认 5000

**使用场景：** 无连接通信、广播

#### 4. 串口

```ini
protocol=serial:///COM5?baudrate=115200&databits=8&stopbits=1&parity=none
```

**参数说明：**
- `baudrate`: 波特率（9600/19200/38400/57600/115200/...）
- `databits`: 数据位（5/6/7/8），默认 8
- `stopbits`: 停止位（1/2），默认 1
- `parity`: 校验位（none/odd/even/mark/space），默认 none

**使用场景：** 串口设备、工业协议

---

## 帧格式配置

### 基本参数

```ini
[default]
; 帧头（HEX格式）
frame_header=FE

; 帧尾（HEX格式）
frame_tail=FF

; 帧长度（字节数）
frame_length=4

; 通道号在帧中的索引（0-based）
channel_index=1

; 值在帧中的索引（0-based）
value_index=2

; 通道号偏移量（0=从0开始，1=从1开始）
channel_offset=0

; 输入通道保持时间（毫秒，0=永久保持）
input_hold_ms=1000
```

### 值编码配置

支持输入输出使用不同的值编码（适用于特殊协议）：

```ini
; 输出值编码（发送DO时使用）
output_value_on_code=01   ; 高电平编码
output_value_off_code=00  ; 低电平编码

; 输入值编码（接收DI时使用）
input_value_on_code=01    ; 高电平编码
input_value_off_code=00   ; 低电平编码
```

### 帧格式示例

**标准协议（如展研 SERIAL-IOFE）：**
```
帧格式: [FE] [通道号] [值] [FF]
示例:   [FE] [05] [01] [FF]  → 通道5 = 1
```

**淘达电子协议：**
```
帧格式: [AA] [通道号] [值] [55]
特点:   通道从1开始，输出值 01=高/02=低
示例:   [AA] [01] [01] [55]  → 通道0 = 1（硬件通道1）
```

---

## 输入输出映射

### 工作模式

IOHUB 支持三种工作模式：

#### 模式1: 显式映射（Explicit Mapping）

在 `[InputMapping]` 和 `[OutputMapping]` 中明确配置：

```ini
[InputMapping]
0=START
1=STOP
10=0xFE01AAFF

[OutputMapping]
0=CMD_OPEN
1=CMD_CLOSE
```

**特点：**
- 精确匹配
- 优先级最高
- 适合固定命令字符串

#### 模式2: 帧解析（Frame Parsing）

配置帧格式，自动解析：

```ini
frame_header=FE
frame_tail=FF
frame_length=4
channel_index=1
value_index=2
```

**特点：**
- 自动解析通道和值
- 支持大量通道
- 适合结构化数据

#### 模式3: 混合模式（推荐）

同时使用两种模式：

```ini
[InputMapping]
; 重要命令使用显式映射
0=EMERGENCY_STOP
1=RESET

; 其他通道 2-255 使用帧解析
```

### 处理优先级

**输入（DI）处理流程：**
```
接收数据
  ↓
1. 提取完整帧（如果配置了帧格式）
  ↓
2. 查找 InputMapping 显式映射
  ↓
3. 如果未找到，尝试帧解析（parseFrame）
  ↓
4. 更新通道状态和时间戳
```

**输出（DO）处理流程：**
```
SetDeviceDO(通道, 值)
  ↓
1. 查找 OutputMapping 显式映射
  ↓
2. 如果未找到，使用帧格式构建（buildFrame）
  ↓
3. 发送数据
```

---

## 支持的协议

### 1. 展研 SERIAL-IOFE 协议

**配置示例：**
```ini
[device_10]
protocol=serial:///COM1?baudrate=115200

frame_header=FE
frame_tail=FF
frame_length=4
channel_index=1
value_index=2
channel_offset=0
```

**协议特点：**
- 帧格式: `FE [通道] [值] FF`
- 通道从 0 开始
- 值编码: 01=高电平, 00=低电平
- 波特率: 115200

**数据示例：**
- 输入: `FE 05 01 FF` → 通道5 = 1
- 输出: `SetDeviceDO(5, 1)` → 发送 `FE 05 01 FF`

### 2. 淘达电子 SERIAL-TDDZ 协议

**配置示例：**
```ini
[device_11]
protocol=serial:///COM2?baudrate=57600

frame_header=AA
frame_tail=55
frame_length=4
channel_index=1
value_index=2
channel_offset=1

; 输出值编码（特殊）
output_value_on_code=01
output_value_off_code=02

; 输入值编码（标准）
input_value_on_code=01
input_value_off_code=00
```

**协议特点：**
- 帧格式: `AA [通道] [值] 55`
- 通道从 1 开始（硬件），API 仍从 0 开始
- 输出值编码特殊: 01=高电平, 02=低电平
- 输入值编码标准: 01=高电平, 00=低电平
- 波特率: 57600

**数据示例：**
- 输出: `SetDeviceDO(0, 1)` → 发送 `AA 01 01 55`（通道1，高电平）
- 输出: `SetDeviceDO(0, 0)` → 发送 `AA 01 02 55`（通道1，低电平）
- 输入: `AA 06 01 55` → 通道5 = 1（硬件通道6）
- 输入: `AA 06 00 55` → 通道5 = 0（硬件通道6）

### 协议对比表

| 特性 | SERIAL-IOFE | SERIAL-TDDZ |
|------|-------------|-------------|
| 帧头 | `0xFE` | `0xAA` |
| 帧尾 | `0xFF` | `0x55` |
| 波特率 | 115200 | 57600 |
| 通道起始 | 0 | 1 |
| channel_offset | 0 | 1 |
| 输出编码 | 01/00 | 01/02 ⚡ |
| 输入编码 | 01/00 | 01/00 |

---

## 配置示例

### 示例1: 纯 ASCII 命令系统

```ini
[default]
protocol=tcp://192.168.1.100:8080
data_format=ascii
write_wait_ms=60

[InputMapping]
0=CMD_START
1=CMD_STOP
2=CMD_RESET
3=CMD_PAUSE

[OutputMapping]
0=START
1=STOP
2=RESET
```

**说明：** 不使用帧解析，所有 IO 通过显式映射。

### 示例2: 纯串口帧协议

```ini
[default]
protocol=serial:///COM5?baudrate=115200

frame_header=FE
frame_tail=FF
frame_length=4
channel_index=1
value_index=2
input_hold_ms=1000
```

**说明：** 不配置映射，所有数据通过帧自动解析，支持 255 个通道。

### 示例3: 混合模式（推荐）

```ini
[default]
protocol=serial:///COM5?baudrate=115200

frame_header=FE
frame_tail=FF
frame_length=4
channel_index=1
value_index=2
input_hold_ms=1000

[InputMapping]
; 紧急命令使用显式映射
0=EMERGENCY_STOP
10=0xFE0AAAFF
11=0xFE0BBBFF

; 通道 1-9, 12-255 通过帧自动解析

[OutputMapping]
0=CMD_EMERGENCY
5=0xFE0501FF
```

**说明：** 重要命令使用显式映射，其他通道自动解析。

### 示例4: 多设备混合协议

```ini
[default]
protocol=udp://127.0.0.1:9000?localport=5000
write_wait_ms=60
input_hold_ms=1000

[device_0]
; TCP 客户端
protocol=tcp://192.168.1.100:8080

[device_1]
; UDP
protocol=udp://192.168.1.200:9000

[device_10]
; 展研协议
protocol=serial:///COM1?baudrate=115200
frame_header=FE
frame_tail=FF
channel_offset=0

[device_11]
; 淘达电子协议
protocol=serial:///COM2?baudrate=57600
frame_header=AA
frame_tail=55
channel_offset=1
output_value_on_code=01
output_value_off_code=02
```

**说明：** 同时管理多个不同协议的设备。

---

## 高级特性

### 1. 输入保持机制

```ini
input_hold_ms=1000  ; 1秒后自动清零
input_hold_ms=0     ; 永久保持，不自动清零
```

**工作流程：**
```
t=0ms:    接收 [FE][05][01][FF] → 通道5 = 1
t=500ms:  通道5 保持 = 1
t=1000ms: 超时，通道5 自动 = 0（如果 input_hold_ms=1000）
```

**应用场景：**
- `input_hold_ms=1000`: 按键检测、脉冲信号
- `input_hold_ms=0`: 开关状态、永久信号

### 2. 通道号偏移

适用于通道编号不从 0 开始的协议：

```ini
channel_offset=0  ; 通道从 0 开始（标准）
channel_offset=1  ; 通道从 1 开始（淘达电子）
channel_offset=10 ; 通道从 10 开始（自定义）
```

**转换逻辑：**
```
发送时: API通道号 + offset = 帧中通道号
接收时: 帧中通道号 - offset = API通道号

示例（offset=1）:
  SetDeviceDO(0, 1) → 帧: AA 01 01 55  (API通道0 → 硬件通道1)
  接收: AA 06 01 55  → API通道5        (硬件通道6 → API通道5)
```

### 3. 值编码映射

支持输入输出使用不同的值编码：

```ini
; 标准协议（输入输出一致）
output_value_on_code=01
output_value_off_code=00
input_value_on_code=01
input_value_off_code=00

; 淘达电子（输入输出不同）
output_value_on_code=01
output_value_off_code=02  ; 特殊
input_value_on_code=01
input_value_off_code=00   ; 标准
```

**转换逻辑：**
```
输出时:
  API值 0 → output_value_off_code
  API值 1 → output_value_on_code

输入时:
  帧值 == input_value_off_code → API值 0
  帧值 == input_value_on_code  → API值 1
```

### 4. 配置继承

设备配置会继承 `[default]` 的设置：

```ini
[default]
frame_header=FE
frame_tail=FF
output_value_on_code=01
output_value_off_code=00

[device_10]
; 继承 default 的所有设置
protocol=serial:///COM1

[device_11]
; 继承 default，但覆盖部分设置
protocol=serial:///COM2
frame_header=AA  ; 覆盖
frame_tail=55    ; 覆盖
output_value_off_code=02  ; 覆盖
```

### 5. 数据格式

支持多种数据格式：

```ini
data_format=auto  ; 自动检测（默认）
data_format=ascii ; ASCII 字符串
data_format=hex   ; HEX 字节序列
```

**格式示例：**
```ini
[InputMapping]
; ASCII
0=START
1=STOP

; HEX（多种写法）
10=0xFE01AAFF
11=FE02BBFF
12=FE 03 CC FF
```

---

## 调试建议

### 启用调试输出
### 常见问题排查

**问题1: 淘达电子设备不工作**
- 检查 `frame_header=AA` 和 `frame_tail=55` 是否配置
- 检查 `channel_offset=1`
- 检查 `output_value_off_code=02`


## 版本历史

### v2.x.y (当前版本)
- ✅ 添加淘达电子协议支持
- ✅ 支持输入输出不同的值编码
- ✅ 支持通道号偏移
- ✅ 修复 TCP Server 协议
- ✅ 添加输入保持机制（input_hold_ms）
- ✅ 统一帧格式支持所有协议
- ✅ 修复设备索引配置加载问题

---

## 技术支持

如有问题，请查看：
1. 配置文件示例：`config.ini`
2. 调试输出日志
3. API 文档：`IOUI.h`

---

**最后更新：** 2025-10-16