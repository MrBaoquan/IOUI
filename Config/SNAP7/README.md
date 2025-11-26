# SNAP7 模块配置指南

SNAP7 模块用于与西门子 S7 系列 PLC 通信，支持开关量（DI/DO）和模拟量（AI）的读写。

---

## 📋 目录

- [快速开始](#快速开始)
- [地址格式说明](#地址格式说明)
- [S7-200 特别说明](#s7-200-特别说明-)
- [连接配置](#连接配置)
- [DI 配置（开关量输入）](#di-配置开关量输入)
- [DO 配置（开关量输出）](#do-配置开关量输出)
- [特殊功能通道](#特殊功能通道)
- [AI 配置（模拟量输入）](#ai-配置模拟量输入)
- [表达式支持](#表达式支持)
- [API 使用](#api-使用)
- [常见问题](#常见问题)

---

## 快速开始

### 1. 基本配置

编辑 `config.ini`：

```ini
[default]
ip = 192.168.2.1
rack = 0
slot = 1

di_area = PE
di_bytes = 8

do_area = PA
do_bytes = 8

ai_channel_0 = DB1.0, int16, x*0.1
```

### 2. 使用代码

```cpp
#include "IOUI.h"

// 打开设备
OpenDevice(0);

// 读取开关量输入
BYTE diStatus[255];
GetDeviceDI(0, diStatus);

// 读取模拟量输入
int32_t aiData[255];
GetDeviceAD_INT(0, aiData);

// 写入开关量输出
short doStatus[255] = {0};
doStatus[0] = 1;  // 设置 Q0.0
SetDeviceDO(0, doStatus);

// 关闭设备
CloseDevice(0);
```

---

## 地址格式说明

### S7 存储区域类型

SNAP7 使用内部区域标识符与 PLC 标准地址对应：

| SNAP7区域 | PLC标准表示 | 说明 | 典型用途 |
|----------|-----------|------|---------|
| **PE** | I/E (Input) | 物理输入区 | 读取传感器、开关、按钮状态 |
| **PA** | Q/A (Output) | 物理输出区 | 控制继电器、阀门、指示灯 |
| **MK** | M (Marker) | 标志位区 | 内部变量、中间结果存储 |
| **DB** | DB (Data Block) | 数据块 | 结构化数据、配方、工艺参数 |

**区域对应关系：**

```
配置使用        PLC程序中           TIA Portal/STEP 7
────────────────────────────────────────────────────────
PE (Process Input)   → I/E   → IB0, IW10, ID20 (输入字节/字/双字)
PA (Process Output)  → Q/A   → QB0, QW10, QD20 (输出字节/字/双字)
MK (Marker)          → M     → MB0, MW10, MD20 (标志字节/字/双字)
DB (Data Block)      → DB    → DB1.DBB0, DB1.DBW10 (数据块)
```

**S7-200 特殊说明：**
- S7-200 的 **V 区**（Variable Memory）通过 **DB1** 访问
- 例如：`VB100` 对应配置为 `DB1.100`

---

### 地址表示法对照

PLC地址有**完整格式**和**简化格式**两种写法，它们指向相同的物理地址：

| 完整格式 | 简化格式 | 含义 | 字节数 |
|---------|---------|------|--------|
| `DB1.DBB100` | `DB1.100` | DB1的字节100 | 1字节 |
| `DB1.DBW100` | `DB1.100` | DB1的字100（从字节100开始） | 2字节 |
| `DB1.DBD100` | `DB1.100` | DB1的双字100（从字节100开始） | 4字节 |
| `DB1.DBX100.5` | `DB1.100.5` | DB1字节100的第5位 | 1位 |

**重要概念：**
- 所有地址都是基于**字节偏移**
- `DBB`/`DBW`/`DBD`/`DBX` 是**数据类型标识**，本模块会自动忽略，只提取数字部分
- `DB1.100` = `DB1.DBB100` = `DB1.DBW100` = 都表示从DB1的第100字节开始
- 实际读写的字节数由配置的数据类型（int16/int32/float32）决定

**⚠️ 注意：**
- 本模块**不支持** `DB1.DBB100.0` 这种语法（某些库如 HslCommunication 支持）
- 位访问请使用 `DB1.100.5` 格式（省略类型前缀）

---

### 地址详细示例

#### 1. DB块地址（最常用）

**字节地址（Byte）：**
```
DB1.DBB0   = DB1的第0字节  (1字节)
DB1.DBB100 = DB1的第100字节 (1字节)
DB1.100    = DB1.DBB100 的简化写法
```

**字地址（Word）：**
```
DB1.DBW0   = DB1的第0-1字节  (2字节)
DB1.DBW10  = DB1的第10-11字节 (2字节)
DB1.10     = 可以表示字节10，也可以表示从字节10开始的字
```

**双字地址（DWord）：**
```
DB1.DBD0   = DB1的第0-3字节   (4字节)
DB1.DBD100 = DB1的第100-103字节 (4字节)
DB1.100    = 从字节100开始
```

**位地址（Bit）：**
```
DB1.DBX0.0   = DB1字节0的第0位
DB1.DBX0.7   = DB1字节0的第7位
DB1.DBX100.3 = DB1字节100的第3位
DB1.100.5    = DB1.DBX100.5 的简化写法（省略DBX）

注意：只有 DBX 前缀才能带位偏移（.0-.7）
     DBB/DBW/DBD 表示字节/字/双字，不能带位偏移
```

#### 2. 物理输入/输出（I/Q 区）

**输入区（I/E/PE）：**
```
IB0     = 输入字节0   (Input Byte 0)
IW64    = 输入字64    (Input Word 64, 从字节64开始)
ID100   = 输入双字100 (Input DWord 100, 从字节100开始)
IX0.5   = 输入字节0的第5位
I0.5    = IX0.5 的简化写法

PE.0    = 输入字节0   (Process Input, byte 0)
PE.64   = 输入字节64
```

**输出区（Q/A/PA）：**
```
QB0     = 输出字节0   (Output Byte 0)
QW10    = 输出字10    (Output Word 10, 从字节10开始)
QD20    = 输出双字20  (Output DWord 20, 从字节20开始)
QX0.0   = 输出字节0的第0位 (Q0.0)
Q0.7    = QX0.7 的简化写法

PA.0    = 输出字节0   (Process Output, byte 0)
PA.10   = 输出字节10
```

#### 3. 标志位区（M/MK）

```
MB0     = 标志位字节0  (Marker Byte 0)
MW10    = 标志位字10   (Marker Word 10, 从字节10开始)
MD20    = 标志位双字20 (Marker DWord 20, 从字节20开始)
MX5.3   = 标志位字节5的第3位
M5.3    = MX5.3 的简化写法

MK.10   = 标志位字节10
```

---

### 配置示例对照

**读取温度（AI）：**
```ini
# 以下三种写法等效（都读取DB1从字节20开始的2字节）
ai_channel_0 = DB1.20, int16, x*0.1        # 简化格式
ai_channel_0 = DB1.DBW20, int16, x*0.1     # 完整格式
ai_channel_0 = DB1.DBW20, int16, x*0.1     # 明确指定读取Word
```

**写入DO位：**
```ini
# 位模式下
do_mode = bit
do_area = DB
do_start = 100
# 通道0 → DB1.100.0 (DB1.DBX100.0)
# 通道7 → DB1.100.7 (DB1.DBX100.7)
# 通道8 → DB1.101.0 (DB1.DBX101.0)
```

**写入DO字节：**
```ini
# 字节模式下
do_mode = byte
do_area = DB
do_start = 100
# 通道0 → DB1.100 (DB1.DBB100)
# 通道5 → DB1.105 (DB1.DBB105)
```

**写入DO字：**
```ini
# 字模式下
do_mode = word
do_area = DB
do_start = 100
# 通道0 → DB1.100-101 (DB1.DBW100)
# 通道1 → DB1.101-102 (DB1.DBW101)
```

---

### 地址计算规则

**通用公式：**
```
实际字节地址 = 起始地址 + 偏移量

位地址：
  字节地址 = 起始地址 + (通道号 ÷ 8)
  位偏移   = 通道号 % 8

字节地址：
  字节地址 = 起始地址 + 通道号

字地址：
  字节地址 = 起始地址 + 通道号（每个通道写2字节）

双字地址：
  字节地址 = 起始地址 + (通道号 ÷ 2)（每2个通道写4字节）
```

**实例计算：**
```ini
do_start = 100
do_mode = bit

通道0  → 字节地址 = 100 + (0÷8) = 100, 位偏移 = 0%8 = 0  → DB1.100.0
通道8  → 字节地址 = 100 + (8÷8) = 101, 位偏移 = 8%8 = 0  → DB1.101.0
通道15 → 字节地址 = 100 + (15÷8) = 101, 位偏移 = 15%8 = 7 → DB1.101.7
```

---

### 常见地址表示对照表

| PLC标准写法 | SNAP7简化格式 | 说明 |
|------------|--------------|------|
| DB1.DBB0 | DB1.0 | DB1字节0 |
| DB1.DBW100 | DB1.100 | DB1字100（字节100-101） |
| DB1.DBD200 | DB1.200 | DB1双字200（字节200-203） |
| DB1.DBX10.5 | DB1.10.5 | DB1字节10的第5位 |
| IW64 | PE.64 或 IW64 | 输入字64（字节64-65） |
| QB0 | PA.0 或 QB0 | 输出字节0 |
| QW10 | PA.10 或 QW10 | 输出字10（字节10-11） |
| MW20 | MK.20 或 MW20 | 标志位字20（字节20-21） |
| Q0.0 | QX0.0 或 Q0.0 | 输出位Q0.0 |

---

## S7-200 特别说明 ⚠️

### V 区访问规则

S7-200 的 **V 区**（Variable Memory）在 SNAP7 中通过 **DB1** 访问：

| S7-200 地址 | SNAP7 配置地址 | 说明 |
|------------|--------------|------|
| `VB0` | `DB1.0` | V区字节0 |
| `VB100` | `DB1.100` | V区字节100 |
| `VW100` | `DB1.100` | V区字100（字节100-101） |
| `VD200` | `DB1.200` | V区双字200（字节200-203） |
| `V0.5` | `DB1.0.5` | V区字节0的第5位 |

### 物理 I/O 访问

S7-200 的物理输入输出可以直接使用 PE/PA：

| S7-200 地址 | SNAP7 配置 | 说明 |
|------------|----------|------|
| `I0.0` | `di_area = PE, di_start = 0` | 输入字节0 |
| `Q0.0` | `do_area = PA, do_start = 0` | 输出字节0 |
| `IB0` | `PE.0` | 输入字节0 |
| `QB0` | `PA.0` | 输出字节0 |

### 详细地址对照表

#### S7-200 各类型数据访问

| 数据类型 | S7-200 地址 | SNAP7 配置 | 字节数 | 说明 |
|---------|-----------|----------|-------|------|
| V区字节 | `VB100` | `ai_channel_0 = DB1.100, uint8` | 1 | 0-255 |
| V区字 | `VW100` | `ai_channel_0 = DB1.100, int16` | 2 | -32768~32767 |
| V区双字 | `VD100` | `ai_channel_0 = DB1.100, int32` | 4 | ±2^31 |
| V区实数 | `VD100` | `ai_channel_0 = DB1.100, float32` | 4 | 浮点数 |
| 物理输入 | `IB0` | `di_area = PE, di_start = 0` | 1 | I0.0-I0.7 |
| 物理输出 | `QB0` | `do_area = PA, do_start = 0` | 1 | Q0.0-Q0.7 |
| 标志位 | `MB0` | `ai_channel_0 = MK.0, uint8` | 1 | M0.0-M0.7 |
| 定时器 | `T0` | `ai_channel_0 = DB1.T0, int16` | 2 | 定时器当前值 |
| 计数器 | `C0` | `ai_channel_0 = DB1.C0, int16` | 2 | 计数器当前值 |

### 配置示例

#### 示例1：基本 I/O 配置

```ini
[default]
ip = 192.168.1.100
rack = 0
slot = 1

# 读取物理输入 I0.0-I0.7（IB0）
di_area = PE
di_db_number = 0
di_start = 0
di_bytes = 1

# 控制物理输出 Q0.0-Q0.7（QB0）
do_area = PA
do_db_number = 0
do_start = 0
do_bytes = 1
do_mode = bit
```

#### 示例2：V 区数据采集

```ini
[default]
ip = 192.168.1.100
rack = 0
slot = 1

# 温度传感器：VW100（整数，需要缩放）
ai_channel_0 = DB1.100, int16, x*0.1

# 压力传感器：VW102（整数）
ai_channel_1 = DB1.102, uint16

# 流量计：VD200（实数）
ai_channel_2 = DB1.200, float32

# 生产计数：VD210（双字整数）
ai_channel_3 = DB1.210, int32

# 转速：VW220（字）
ai_channel_4 = DB1.220, uint16, x*60/1024
```

#### 示例3：多种数据源混合

```ini
[default]
ip = 192.168.1.100
rack = 0
slot = 1

# 物理输入
di_area = PE
di_start = 0
di_bytes = 2            # I0.0-I1.7

# 物理输出
do_area = PA
do_start = 0
do_bytes = 2            # Q0.0-Q1.7

# V区模拟量
ai_channel_0 = DB1.100, int16, x*0.1    # VW100: 温度
ai_channel_1 = DB1.102, int16, x*0.01   # VW102: 湿度

# 标志位区（读取PLC内部状态）
ai_channel_2 = MK.0, uint8              # MB0: 状态字
ai_channel_3 = MK.10, int16             # MW10: 内部计数
```

#### 示例4：写入 V 区数据（特殊功能通道）

```cpp
// 写入 VB200 = 128
short doStatus[255] = {0};
doStatus[240] = 1;     // 区域码: DB
doStatus[241] = 1;     // DB号: 1
doStatus[242] = 200;   // 地址: 200 (VB200)
doStatus[243] = 1;     // 功能码: 1=写字节
doStatus[244] = 128;   // 数据: 128
SetDeviceDO(0, doStatus);

// 写入 VW202 = 1000
doStatus[240] = 1;     // 区域码: DB
doStatus[241] = 1;     // DB号: 1
doStatus[242] = 202;   // 地址: 202 (VW202)
doStatus[243] = 2;     // 功能码: 2=写字(2字节)
doStatus[244] = 1000 & 0xFF;        // 低字节
doStatus[245] = (1000 >> 8) & 0xFF; // 高字节
SetDeviceDO(0, doStatus);
```

---

## 连接配置

### 基本参数

| 参数 | 说明 | S7-200 | S7-300/400 | S7-1200/1500 |
|------|------|--------|-----------|-------------|
| `rack` | 机架号 | 0 | 0 | 0 |
| `slot` | 槽号 | 1 | 2 | 1 |
| `timeout_ms` | 连接超时（毫秒） | 1000 | 1000 | 1000 |
| `poll_interval_ms` | 轮询间隔（毫秒） | 100 | 100 | 100 |

### 不同型号配置示例

#### S7-200 完整配置

```ini
[default]
# ========== 连接参数 ==========
ip = 192.168.1.100
rack = 0
slot = 1                # S7-200 固定为 1
timeout_ms = 1000
retry_wait_ms = 20
poll_interval_ms = 100
command_interval_ms = 10
max_consecutive_failures = 3

# ========== 开关量输入 ==========
# 读取物理输入 I0.0-I0.7（IB0）
di_area = PE
di_db_number = 0
di_start = 0
di_bytes = 1

# ========== 开关量输出 ==========
# 控制物理输出 Q0.0-Q0.7（QB0）
do_area = PA
do_db_number = 0
do_start = 0
do_bytes = 1
do_mode = bit

# ========== 模拟量输入 ==========
# 读取 V 区数据（S7-200 的 V 区通过 DB1 访问）
ai_channel_0 = DB1.100, int16, x*0.1    # VW100: 温度传感器
ai_channel_1 = DB1.102, int16            # VW102: 压力传感器
ai_channel_2 = DB1.200, float32          # VD200: 流量计（浮点数）
```

#### S7-300/400 完整配置

```ini
[default]
# ========== 连接参数 ==========
ip = 192.168.1.200
rack = 0
slot = 2                # S7-300/400 CPU 通常在槽号 2
timeout_ms = 1000
retry_wait_ms = 20
poll_interval_ms = 100
command_interval_ms = 10
max_consecutive_failures = 3

# ========== 开关量输入 ==========
# 读取物理输入 I0.0-I7.7（IB0-IB7）
di_area = PE
di_db_number = 0
di_start = 0
di_bytes = 8            # 8字节 = 64位

# ========== 开关量输出 ==========
# 控制物理输出 Q0.0-Q7.7（QB0-QB7）
do_area = PA
do_db_number = 0
do_start = 0
do_bytes = 8
do_mode = bit

# ========== 模拟量输入 ==========
# 读取数据块
ai_channel_0 = DB10.0, int16, x*0.1     # DB10.DBW0: 温度
ai_channel_1 = DB10.2, int16            # DB10.DBW2: 压力
ai_channel_2 = DB10.10, float32         # DB10.DBD10: 流量（Real）
ai_channel_3 = DB10.20, int32           # DB10.DBD20: 计数器（DInt）
ai_channel_4 = MW100, int16             # M 区：内部变量
```

#### S7-1200/1500 完整配置

```ini
[default]
# ========== 连接参数 ==========
ip = 192.168.1.150
rack = 0
slot = 1                # S7-1200/1500 固定为 1
timeout_ms = 1000
retry_wait_ms = 20
poll_interval_ms = 100
command_interval_ms = 10
max_consecutive_failures = 3

# ========== 开关量输入 ==========
# 读取物理输入 %I0.0-%I7.7
di_area = PE
di_db_number = 0
di_start = 0
di_bytes = 8

# ========== 开关量输出 ==========
# 控制物理输出 %Q0.0-%Q7.7
do_area = PA
do_db_number = 0
do_start = 0
do_bytes = 8
do_mode = bit

# ========== 模拟量输入 ==========
# 读取数据块（支持优化访问的 DB）
ai_channel_0 = DB1.0, float32           # DB1.DBD0: 温度（Real）
ai_channel_1 = DB1.4, float32           # DB1.DBD4: 压力（Real）
ai_channel_2 = DB1.10, int16            # DB1.DBW10: 速度设定
ai_channel_3 = DB1.20, int32            # DB1.DBD20: 生产计数（DInt）
ai_channel_4 = DB2.0, uint16            # DB2.DBW0: 报警代码（Word）
```

### 多设备配置示例

如果需要连接多台不同型号的 PLC：

```ini
# ========== 设备 0: S7-200 ==========
[default]
ip = 192.168.1.100
rack = 0
slot = 1
di_area = PE
di_start = 0
di_bytes = 1
ai_channel_0 = DB1.100, int16, x*0.1

# ========== 设备 1: S7-300 ==========
[device_1]
ip = 192.168.1.200
rack = 0
slot = 2                # 注意槽号改为 2
di_area = PE
di_start = 0
di_bytes = 8
ai_channel_0 = DB10.0, int16, x*0.1

# ========== 设备 2: S7-1200 ==========
[device_2]
ip = 192.168.1.150
rack = 0
slot = 1
di_area = PE
di_start = 0
di_bytes = 8
ai_channel_0 = DB1.0, float32
```

---

## DI 配置（开关量输入）

### 配置参数

| 参数 | 说明 | 常用值 |
|------|------|--------|
| `di_area` | 存储区域 | PE(输入)/PA(输出)/MK(标志位)/DB(数据块) |
| `di_db_number` | DB块号 | PE/PA/MK填0, DB区填块号 |
| `di_start` | 起始地址 | 字节偏移（0=从首地址开始） |
| `di_bytes` | 读取字节数 | 8字节=64位 |

**di_area 区域说明：**

| 配置值 | PLC对应区域 | 典型应用 | 示例 |
|-------|-----------|---------|------|
| **PE** | I/E (物理输入) | 传感器、开关、按钮 | 接近开关、限位开关 |
| **PA** | Q/A (物理输出) | 读取输出状态反馈 | 检查阀门是否打开 |
| **MK** | M (标志位) | 读取PLC内部变量 | 运行状态、报警标志 |
| **DB** | DB (数据块) | 读取结构化数据 | 生产计数、工艺参数 |

### 配置示例

```ini
# 示例1: 读取物理输入（I0.0-I7.7，对应 IB0-IB7）
di_area = PE          # 物理输入区 (Process Input)
di_db_number = 0      # PE区不使用DB号
di_start = 0          # 从 IB0 开始
di_bytes = 8          # 读取 8 字节（64 位）

# 示例2: 读取物理输出状态（Q0.0-Q7.7，对应 QB0-QB7）
di_area = PA          # 物理输出区 (Process Output)
di_db_number = 0
di_start = 0          # 从 QB0 开始
di_bytes = 8

# 示例3: 读取标志位（M0.0-M7.7，对应 MB0-MB7）
di_area = MK          # 标志位区 (Marker)
di_db_number = 0
di_start = 0          # 从 MB0 开始
di_bytes = 8

# 示例4: 从DB块读取（DB100.DBB0-DBB7）
di_area = DB          # 数据块区 (Data Block)
di_db_number = 100    # DB100
di_start = 0          # 从 DB100.DBB0 开始
di_bytes = 8
```

### 使用代码

```cpp
BYTE diStatus[255] = {0};
GetDeviceDI(0, diStatus);

if (diStatus[0]) printf("通道0 = ON\n");   // I0.0 或 DB100.0.0
if (diStatus[8]) printf("通道8 = ON\n");   // I1.0 或 DB100.1.0
```

---

## DO 配置（开关量输出）

### 输出模式（do_mode）

SNAP7 支持 4 种输出模式，通过 `do_mode` 参数配置：

| 模式值 | 模式名称 | 通道含义 | 适用场景 |
|--------|---------|---------|---------|
| **0** 或 **bit** | 位模式（默认） | 通道索引 → 位地址 | 开关量控制、继电器 |
| **1** 或 **byte** | 字节模式 | 通道索引 → 字节地址 | PWM占空比、LED亮度 |
| **2** 或 **word** | 字模式 | 通道索引 → 字地址 | 模拟量输出、速度设定 |
| **3** 或 **dword** | 双字模式 | 2个通道 → 1个双字地址 | 计数器、位置控制 |

---

### 模式 0：位模式（BitMode）⭐ 默认

**特点：** 连续区域按位输出，8个通道对应1个字节。

**参数说明：**
| 参数 | 说明 | 常用值 |
|------|------|--------|
| `do_mode` | 输出模式 | bit (或 0) |
| `do_area` | 存储区域 | PA(输出)/DB(数据块)/MK(标志位) |
| `do_db_number` | DB块号 | PA/MK填0, DB区填块号 |
| `do_start` | 起始地址 | 字节偏移 |
| `do_bytes` | 写入字节数 | 8字节=64位 |

**do_area 区域说明（位模式）：**

| 配置值 | PLC对应区域 | 典型应用 | 示例 |
|-------|-----------|---------|------|
| **PA** | Q/A (物理输出) | 控制继电器、阀门、指示灯 | 气缸控制、电机启停 |
| **MK** | M (标志位) | 设置PLC内部变量 | 启动信号、模式切换 |
| **DB** | DB (数据块) | 写入数据块位 | 配方选择、工艺开关 |

**地址映射：**
```
通道 0-7   → 字节 do_start+0 的位 0-7
通道 8-15  → 字节 do_start+1 的位 0-7
通道 16-23 → 字节 do_start+2 的位 0-7
```

**⚠️ 首次写入行为：**
- 位模式下，首次调用 `SetDeviceDO` 会**强制写入所有通道**（包括值为0的通道）
- 这是因为位模式是整块写入，需要初始化整个区域
- 后续调用只写入发生变化的通道

**配置示例：**
```ini
# 示例1: 控制物理输出（Q0.0-Q7.7）
do_mode = bit      # 位模式
do_area = PA       # 物理输出区 (Process Output)
do_db_number = 0
do_start = 0       # 从 QB0 开始
do_bytes = 8       # 控制 64 位（Q0.0-Q7.7）

# 示例2: 控制标志位（M10.0-M17.7）
do_mode = bit
do_area = MK       # 标志位区 (Marker)
do_db_number = 0
do_start = 10      # 从 MB10 开始
do_bytes = 8

# 示例3: 写入DB块位（DB1.DBX100.0-DB1.DBX107.7）
do_mode = bit
do_area = DB       # 数据块区 (Data Block)
do_db_number = 1   # DB1
do_start = 100     # 从 DB1.DBB100 开始
do_bytes = 8
```

**使用代码：**
```cpp
short doStatus[255] = {0};
doStatus[0] = 1;   // 设置 Q0.0（或 M10.0，或 DB1.100.0）
doStatus[7] = 1;   // 设置 Q0.7（或 M10.7，或 DB1.100.7）
doStatus[8] = 1;   // 设置 Q1.0（或 M11.0，或 DB1.101.0）
SetDeviceDO(0, doStatus);
```

---

### 模式 1：字节模式（ByteMode）

**特点：** 通道索引即字节地址，每个通道写入 0-255。

**参数说明：**
| 参数 | 说明 | 常用值 |
|------|------|--------|
| `do_mode` | 输出模式 | byte (或 1) |
| `do_area` | 存储区域 | PA(输出), DB(数据块), MK(标志位) |
| `do_db_number` | DB块号 | PA/MK填0, DB区填块号 |
| `do_start` | 起始地址 | 字节偏移 |

**地址映射：**
```
通道 0  → 地址 do_start+0 (1字节)
通道 5  → 地址 do_start+5 (1字节)
通道 10 → 地址 do_start+10 (1字节)
```

**配置示例：**
```ini
do_mode = byte     # 字节模式
do_area = DB
do_db_number = 1
do_start = 100     # DB1.DBB100 开始
```

**使用代码：**
```cpp
short doStatus[255] = {0};
doStatus[0] = 128;   // DB1.DBB100 = 128 (50% PWM)
doStatus[5] = 255;   // DB1.DBB105 = 255 (100% 亮度)
doStatus[10] = 64;   // DB1.DBB110 = 64 (25% 占空比)
SetDeviceDO(0, doStatus);
```

**⚠️ 首次写入行为：**
- 字节模式采用**惰性写入**，只有用户显式设置的通道才会写入PLC
- 启动时不会写入未设置的通道，避免大量无效操作（240个通道 = 240次网络请求）

---

### 模式 2：字模式（WordMode）

**特点：** 通道索引即字地址，每个通道写入 0-65535（2字节）。

**参数说明：**
| 参数 | 说明 | 常用值 |
|------|------|--------|
| `do_mode` | 输出模式 | word (或 2) |
| `do_area` | 存储区域 | PA(输出), DB(数据块), MK(标志位) |
| `do_db_number` | DB块号 | PA/MK填0, DB区填块号 |
| `do_start` | 起始地址 | 字节偏移 |

**地址映射：**
```
通道 0 → 地址 do_start+0 (2字节, 大端序)
通道 3 → 地址 do_start+3 (2字节, 大端序)
通道 7 → 地址 do_start+7 (2字节, 大端序)
```

**配置示例：**
```ini
do_mode = word     # 字模式
do_area = DB
do_db_number = 1
do_start = 200     # DB1.DBW200 开始
```

**使用代码：**
```cpp
short doStatus[255] = {0};
doStatus[0] = 3000;   // DB1.DBW200 = 3000 (电机转速)
doStatus[3] = 1500;   // DB1.DBW203 = 1500
doStatus[7] = 5000;   // DB1.DBW207 = 5000
SetDeviceDO(0, doStatus);
```

**⚠️ 首次写入行为：**
- 字模式采用**惰性写入**，只写入用户显式设置的通道
- 避免启动时大量无效的网络操作

---

### 模式 3：双字模式（DWordMode）

**特点：** 每 2 个通道组成 1 个双字地址，写入 32 位整数。

**参数说明：**
| 参数 | 说明 | 常用值 |
|------|------|--------|
| `do_mode` | 输出模式 | dword (或 3) |
| `do_area` | 存储区域 | PA(输出), DB(数据块), MK(标志位) |
| `do_db_number` | DB块号 | PA/MK填0, DB区填块号 |
| `do_start` | 起始地址 | 字节偏移 |

**地址映射：**
```
通道 0,1 → 地址 do_start+0 (4字节, 大端序)
通道 2,3 → 地址 do_start+1 (4字节, 大端序)
通道 4,5 → 地址 do_start+2 (4字节, 大端序)
```

**数据组合：**
```
DWord = (通道N << 16) | 通道N+1
```

**配置示例：**
```ini
do_mode = dword    # 双字模式
do_area = DB
do_db_number = 1
do_start = 300     # DB1.DBD300 开始
```

**使用代码：**
```cpp
short doStatus[255] = {0};
// 写入 DB1.DBD300 = 0x12345678
doStatus[0] = 0x1234;  // 高16位
doStatus[1] = 0x5678;  // 低16位

// 写入 DB1.DBD301 = 100000
doStatus[2] = 1;       // 高16位
doStatus[3] = 34464;   // 低16位 (100000 & 0xFFFF)

SetDeviceDO(0, doStatus);
```

**⚠️ 首次写入行为：**
- 双字模式采用**惰性写入**，只写入用户显式设置的通道对
- 避免启动时大量无效的网络操作

---

### 模式选择指南

| 应用场景 | 推荐模式 | 配置示例 | 首次写入策略 |
|---------|---------|---------|-------------|
| 开关量控制（Q点） | 位模式 (bit) | `do_mode=bit, do_area=PA` | 强制全部写入（整块初始化） |
| PWM 占空比 | 字节模式 (byte) | `do_mode=byte, do_area=DB` | 惰性写入（按需写入） |
| 电机转速设定 | 字模式 (word) | `do_mode=word, do_area=DB` | 惰性写入（按需写入） |
| 计数器/位置 | 双字模式 (dword) | `do_mode=dword, do_area=DB` | 惰性写入（按需写入） |

---

## 特殊功能通道

通道 240-249 用于动态写入任意地址，无需修改配置文件。

### 通道分配（层级结构）

**通道 240：区域码**
- `1` = DB区（数据块）
- `2` = PA区（物理输出）
- `3` = PE区（物理输入）
- `4` = MK区（标志位）

**DB区参数布局：**
| 通道 | 用途 | 说明 |
|------|------|------|
| 241 | DB号 | DB块号（1-N） |
| 242 | 地址 | 字节偏移 |
| 243 | 功能码 | 1=字节, 2=字, 3=双字, 4=位 |
| 244+ | 数据 | 根据功能码不同，占用1-4个通道 |

**PA/PE/MK区参数布局：**
| 通道 | 用途 | 说明 |
|------|------|------|
| 241 | 地址 | 字节偏移 |
| 242 | 功能码 | 1=字节, 2=字, 3=双字, 4=位 |
| 243+ | 数据 | 根据功能码不同，占用1-4个通道 |

### 功能码详解

#### 功能码 1：写字节（Byte）

写入单个字节（1字节）。

**DB区示例：写入 DB1.DBB100 = 0x42**
```cpp
short doStatus[255] = {0};
doStatus[240] = 1;      // 区域码: DB区
doStatus[241] = 1;      // DB号: 1
doStatus[242] = 100;    // 地址: 100
doStatus[243] = 1;      // 功能码: 写字节
doStatus[244] = 0x42;   // 数据: 66
SetDeviceDO(0, doStatus);
```

**PA区示例：写入 QB10 = 0xAA**
```cpp
doStatus[240] = 2;      // 区域码: PA区
doStatus[241] = 10;     // 地址: 10
doStatus[242] = 1;      // 功能码: 写字节
doStatus[243] = 0xAA;   // 数据: 170
SetDeviceDO(0, doStatus);
```

#### 功能码 2：写字（Word）

写入2字节（大端序）。

**DB区示例：写入 DB1.DBW50 = 1234**
```cpp
doStatus[240] = 1;      // 区域码: DB区
doStatus[241] = 1;      // DB号: 1
doStatus[242] = 50;     // 地址: 50
doStatus[243] = 2;      // 功能码: 写字
doStatus[244] = 1234;   // 数据: 1234
SetDeviceDO(0, doStatus);
```

**MK区示例：写入 MW20 = 5678**
```cpp
doStatus[240] = 4;      // 区域码: MK区
doStatus[241] = 20;     // 地址: 20
doStatus[242] = 2;      // 功能码: 写字
doStatus[243] = 5678;   // 数据: 5678
SetDeviceDO(0, doStatus);
```

#### 功能码 3：写双字（DWord）

写入4字节（大端序）。

**DB区示例：写入 DB1.DBD60 = 0x12345678**
```cpp
doStatus[240] = 1;        // 区域码: DB区
doStatus[241] = 1;        // DB号: 1
doStatus[242] = 60;       // 地址: 60
doStatus[243] = 3;        // 功能码: 写双字
doStatus[244] = 0x1234;   // 高16位
doStatus[245] = 0x5678;   // 低16位
SetDeviceDO(0, doStatus);
```

#### 功能码 4：写位（Bit）⭐

写入单个位，自动从 lastStatus 缓存重构字节（避免读-改-写操作）。

**DB区示例：写入 DB1.0.3 = 1（设置DB1字节0的第3位）**
```cpp
doStatus[240] = 1;      // 区域码: DB区
doStatus[241] = 1;      // DB号: 1
doStatus[242] = 0;      // 地址: 0（字节0）
doStatus[243] = 4;      // 功能码: 写位
doStatus[244] = 3;      // 位偏移: 3（0-7）
doStatus[245] = 1;      // 位值: 1（0/1）
SetDeviceDO(0, doStatus);
```

**PA区示例：写入 Q1.0 = 1**
```cpp
doStatus[240] = 2;      // 区域码: PA区
doStatus[241] = 1;      // 地址: 1（QB1）
doStatus[242] = 4;      // 功能码: 写位
doStatus[243] = 0;      // 位偏移: 0（Q1.0）
doStatus[244] = 1;      // 位值: 1
SetDeviceDO(0, doStatus);
```

**优势：** 使用 lastStatus 缓存的字节状态，无需从PLC读取，效率更高且避免竞争条件。

---

## AI 配置（模拟量输入）

读取 PLC 的模拟量数据。

### 配置格式

```ini
ai_channel_N = 区域地址, 数据类型[, 表达式][, 字节序]
```

### 区域地址格式

| 格式 | 说明 | 示例 |
|------|------|------|
| DB1.20 | DB块地址 | DB1.DBW20 |
| IW10 | 输入字 | PE.10 |
| QW20 | 输出字 | PA.20 |
| MW30 | 标志位字 | MK.30 |

### 数据类型

| 类型 | 大小 | 范围 | 说明 |
|------|------|------|------|
| uint8 | 1字节 | 0 ~ 255 | 无符号字节 |
| int8 | 1字节 | -128 ~ 127 | 有符号字节 |
| uint16 | 2字节 | 0 ~ 65535 | 无符号整数 |
| int16 | 2字节 | -32768 ~ 32767 | 有符号整数 |
| uint32 | 4字节 | 0 ~ 4294967295 | 无符号长整数 |
| int32 | 4字节 | -2147483648 ~ 2147483647 | 有符号长整数 |
| float32 | 4字节 | ±1.18E-38 ~ ±3.4E38 | 浮点数 |

### 配置示例

```ini
# 温度传感器（PLC存250，实际25.0°C）
ai_channel_0 = DB1.0, int16, x*0.1

# 压力传感器（浮点数）
ai_channel_1 = DB1.4, float32

# 编码器位置（无符号，不转换）
ai_channel_2 = DB1.8, uint32

# 华氏转摄氏
ai_channel_3 = DB1.12, int16, (x-32)*5/9

# 非线性传感器
ai_channel_4 = DB1.14, uint16, sqrt(x)*10

# 状态码（0-255）
ai_channel_5 = DB1.50, uint8

# 温度偏移（-50~50°C）
ai_channel_6 = DB1.51, int8, x*0.5
```

---

## 表达式支持

使用数学表达式对原始数据进行转换。

### 变量

- `x` - 原始值（PLC读取的数值）

### 支持的运算符

| 运算符 | 说明 | 示例 |
|--------|------|------|
| `+` | 加法 | `x+100` |
| `-` | 减法 | `x-32` |
| `*` | 乘法 | `x*0.1` |
| `/` | 除法 | `x/10` |
| `^` | 幂运算 | `x^2` |
| `()` | 括号 | `(x-32)*5/9` |

### 支持的函数

| 函数 | 说明 | 示例 |
|------|------|------|
| `abs(x)` | 绝对值 | `abs(x-100)` |
| `sqrt(x)` | 平方根 | `sqrt(x)*10` |
| `sin(x)`, `cos(x)` | 三角函数 | `sin(x*3.14/180)` |
| `ln(x)`, `log(x)` | 对数 | `ln(x)*100` |
| `exp(x)` | 指数 | `exp(x/1000)` |

### 示例

```ini
# 简单缩放
ai_channel_0 = DB1.0, int16, x*0.1

# 温度转换
ai_channel_1 = DB1.2, int16, (x-32)*5/9

# 非线性转换
ai_channel_2 = DB1.4, uint16, sqrt(x)*10

# 复杂公式
ai_channel_3 = DB1.6, float32, (x*0.1+20)/100
```

---

## API 使用

### 初始化和连接

```cpp
#include "IOUI.h"

// 初始化（获取设备信息）
DeviceInfo* devInfo = Initialize();
printf("InputCount: %d\n", devInfo->InputCount);   // 255
printf("OutputCount: %d\n", devInfo->OutputCount); // 255

// 打开设备（加载配置并连接PLC）
int ret = OpenDevice(0);
if (ret == 0) {
    printf("连接失败\n");
    return;
}
```

### 读取开关量输入

```cpp
BYTE diStatus[255] = {0};
GetDeviceDI(0, diStatus);

// 检查各个输入点状态
for (int i = 0; i < 64; i++) {
    if (diStatus[i]) {
        printf("I%d.%d = ON\n", i/8, i%8);
    }
}
```

### 写入开关量输出

```cpp
short doStatus[255] = {0};

// 设置输出
doStatus[0] = 1;   // Q0.0 = ON
doStatus[1] = 0;   // Q0.1 = OFF
doStatus[7] = 1;   // Q0.7 = ON

SetDeviceDO(0, doStatus);
```

### 读取模拟量输入

```cpp
int32_t aiData[255] = {0};
GetDeviceAD_INT(0, aiData);

// 显示各通道值（已应用表达式转换）
printf("温度: %d °C\n", aiData[0]);      // 例如: 25
printf("压力: %d kPa\n", aiData[1]);     // 例如: 150
printf("位置: %d mm\n", aiData[2]);      // 例如: 1000
```

### 动态写入（特殊功能通道）

```cpp
short doStatus[255] = {0};

// 写字节: DB1.DBB100 = 0x42
doStatus[240] = 1;      // 区域码: DB区
doStatus[241] = 1;      // DB号: 1
doStatus[242] = 100;    // 地址: 100
doStatus[243] = 1;      // 功能码: 写字节
doStatus[244] = 0x42;   // 数据: 66
SetDeviceDO(0, doStatus);

// 写字: DB1.DBW50 = 1234
doStatus[240] = 1;      // 区域码: DB区
doStatus[241] = 1;      // DB号: 1
doStatus[242] = 50;     // 地址: 50
doStatus[243] = 2;      // 功能码: 写字
doStatus[244] = 1234;   // 数据: 1234
SetDeviceDO(0, doStatus);

// 写位: Q1.0 = 1
doStatus[240] = 2;      // 区域码: PA区
doStatus[241] = 1;      // 地址: 1（QB1）
doStatus[242] = 4;      // 功能码: 写位
doStatus[243] = 0;      // 位偏移: 0
doStatus[244] = 1;      // 位值: 1
SetDeviceDO(0, doStatus);
```

### 关闭设备

```cpp
CloseDevice(0);
```

---

## 不同型号 PLC 配置对比总结

### 型号差异对照表

| 特性 | S7-200 | S7-300/400 | S7-1200/1500 |
|------|--------|-----------|-------------|
| **连接参数** | | | |
| Rack（机架号） | 0 | 0 | 0 |
| Slot（槽号） | 1 | 2 ⚠️ | 1 |
| **特有区域** | | | |
| V 区（变量区） | ✅ 有（通过 DB1 访问） | ❌ 无 | ❌ 无 |
| 优化数据块 | ❌ 不支持 | ❌ 不支持 | ✅ 支持 |
| **地址表示** | | | |
| 输入 | I0.0, IB0, IW0 | I0.0, IB0, IW0 | %I0.0, %IB0, %IW0 |
| 输出 | Q0.0, QB0, QW0 | Q0.0, QB0, QW0 | %Q0.0, %QB0, %QW0 |
| 标志位 | M0.0, MB0, MW0 | M0.0, MB0, MW0 | %M0.0, %MB0, %MW0 |
| 数据块 | V 区 → DB1 | DB1, DB2... | DB1, DB2... |
| **数据类型** | | | |
| Byte/Word/DWord | ✅ | ✅ | ✅ |
| Real（浮点） | ✅ 32位 | ✅ 32位 | ✅ 32位 + 64位 |
| String | ✅ | ✅ | ✅ 增强型 |
| **推荐用途** | 小型控制 | 中大型系统 | 现代化系统 |

### 关键配置差异

#### 1. Slot（槽号）配置

```ini
# S7-200
slot = 1

# S7-300/400（最常见的错误！）
slot = 2     # ⚠️ 注意：不是 1

# S7-1200/1500
slot = 1
```

#### 2. 数据块访问差异

**S7-200:**
```ini
# V 区必须通过 DB1 访问
ai_channel_0 = DB1.100, int16    # 对应 VW100
ai_channel_1 = DB1.200, float32  # 对应 VD200
```

**S7-300/400/1200/1500:**
```ini
# 直接访问真正的 DB 块
ai_channel_0 = DB10.0, int16     # DB10.DBW0
ai_channel_1 = DB10.4, float32   # DB10.DBD4
```

#### 3. 地址范围差异

| 型号 | 输入区大小 | 输出区大小 | 标志位区大小 | 数据块数量 |
|------|----------|----------|------------|----------|
| S7-200 | I0.0-I15.7 | Q0.0-Q15.7 | M0.0-M31.7 | DB1（V区） |
| S7-300 | I0.0-I127.7 | Q0.0-Q127.7 | M0.0-M255.7 | DB1-DB255 |
| S7-400 | I0.0-I511.7 | Q0.0-Q511.7 | M0.0-M4095.7 | DB1-DB65535 |
| S7-1200 | I0.0-I127.7 | Q0.0-Q127.7 | M0.0-M255.7 | DB1-DB60000 |
| S7-1500 | I0.0-I511.7 | Q0.0-Q511.7 | M0.0-M4095.7 | DB1-DB60000 |

### 实际应用场景配置示例

#### 场景1：温度监控系统（S7-200）

```ini
[default]
ip = 192.168.1.100
rack = 0
slot = 1

# 8路温度传感器（VW0-VW14，间隔2字节）
ai_channel_0 = DB1.0, int16, x*0.1
ai_channel_1 = DB1.2, int16, x*0.1
ai_channel_2 = DB1.4, int16, x*0.1
ai_channel_3 = DB1.6, int16, x*0.1
ai_channel_4 = DB1.8, int16, x*0.1
ai_channel_5 = DB1.10, int16, x*0.1
ai_channel_6 = DB1.12, int16, x*0.1
ai_channel_7 = DB1.14, int16, x*0.1

# 8路报警输出
do_area = PA
do_start = 0
do_bytes = 1
```

#### 场景2：生产线控制（S7-300）

```ini
[default]
ip = 192.168.1.200
rack = 0
slot = 2        # ⚠️ S7-300 使用槽号 2

# 64路开关量输入（传感器状态）
di_area = PE
di_start = 0
di_bytes = 8

# 64路开关量输出（执行器控制）
do_area = PA
do_start = 0
do_bytes = 8

# 工艺参数（DB10）
ai_channel_0 = DB10.0, float32      # 速度设定
ai_channel_1 = DB10.4, float32      # 温度设定
ai_channel_2 = DB10.8, int32        # 生产计数
ai_channel_3 = DB10.12, int32       # 良品计数
ai_channel_4 = DB10.16, int32       # 次品计数
```

#### 场景3：现代化工厂（S7-1200）

```ini
[default]
ip = 192.168.1.150
rack = 0
slot = 1

# 数字输入（模块化扩展）
di_area = PE
di_start = 0
di_bytes = 16       # 128路输入

# 数字输出
do_area = PA
do_start = 0
do_bytes = 16       # 128路输出

# 过程数据（优化DB块）
ai_channel_0 = DB1.0, float32       # 流量
ai_channel_1 = DB1.4, float32       # 压力
ai_channel_2 = DB1.8, float32       # 温度
ai_channel_3 = DB1.12, float32      # 液位

# 配方数据
ai_channel_10 = DB2.0, int16        # 配方号
ai_channel_11 = DB2.2, float32      # 参数1
ai_channel_12 = DB2.6, float32      # 参数2
```

---

## 常见问题

### Q1: 连接失败怎么办？

**A**: 检查以下几点：
1. PLC IP地址是否正确（ping 测试）
2. Rack 和 Slot 是否匹配 PLC 配置
3. PLC 是否开启了 S7 通信
4. 防火墙是否阻止了 102 端口

### Q2: 读取的数据不正确？

**A**: 确认：
1. 地址配置是否正确（DB号、偏移量）
2. 数据类型是否匹配（int16/uint16/float32）
3. 字节序是否正确（S7 默认大端序 AB）
4. 表达式是否正确（检查缩放因子）

### Q3: 写入没有生效？

**A**: 检查：
1. PLC 是否在运行状态
2. DB块是否在 PLC 程序中定义
3. 地址是否超出 DB 块范围
4. DO 区域配置是否正确

### Q4: 特殊功能通道不工作？

**A**: 注意：
1. 区域码（通道240）必须设置正确（1=DB, 2=PA, 3=PE, 4=MK）
2. 参数顺序根据区域不同：
   - DB区：通道241=DB号, 242=地址, 243=功能码, 244+=数据
   - 其他区：通道241=地址, 242=功能码, 243+=数据
3. 每次调用 `SetDeviceDO` 后，特殊通道会被重置为0
4. 功能码4（写位）会自动使用 lastStatus 缓存，无需手动读取

### Q5: 表达式不生效？

**A**: 确保：
1. 表达式语法正确（使用 `x` 作为变量）
2. 没有拼写错误（如 `sqtr` 应为 `sqrt`）
3. 括号匹配
4. 配置文件保存并重启程序

### Q6: 如何查看通信状态？

**A**: 模块内置自动重连机制：
- 连续失败3次后自动断开
- 每5秒尝试重连一次
- 可通过 `max_consecutive_failures` 配置

### Q7: 支持多台 PLC 吗？

**A**: 支持！使用设备索引区分：

```ini
[device_1]
ip = 192.168.1.100

[device_2]
ip = 192.168.1.101
```

```cpp
OpenDevice(1);  // PLC 1
OpenDevice(2);  // PLC 2
```

---

## 技术规格

| 项目 | 规格 |
|------|------|
| 支持 PLC | S7-200/300/400/1200/1500 |
| 通信协议 | S7Comm |
| 最大 DI | 255 通道 |
| 最大 DO | 255 通道 |
| 最大 AI | 255 通道 |
| 轮询周期 | 100ms（可配置） |
| 连接超时 | 1000ms（可配置） |
| 自动重连 | 支持 |
| 线程安全 | 是 |

---

## 版本信息

- **当前版本**: V2.6.0
- **更新日期**: 2025-11-25
- **依赖库**: SNAP7, TinyExpr
- **维护者**: MrBaoquan

---

**需要更多帮助？**
- 查看示例代码：`IOUITester` 项目
- 参考 MODBUS 模块配置（类似结构）
- 查看 TIA Portal 或 STEP 7 帮助文档
