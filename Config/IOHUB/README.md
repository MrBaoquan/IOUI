# IOHUB 配置说明

## 输入映射的两种工作模式

IOHUB 支持两种输入数据处理模式，可以单独使用或混合使用：

### 模式1: 显式映射（Explicit Mapping）

在 `[InputMapping]` 中明确配置每个通道的数据：

```ini
[InputMapping]
0=START
1=STOP
10=0xFE01AAFF
```

**特点：**
- 精确匹配：只有完全匹配的数据才触发通道
- 优先级高：优先于帧解析
- 适用场景：固定的命令字符串、特殊控制码

### 模式2: 帧解析（Frame Parsing）

配置帧格式规则，自动解析结构化数据：

```ini
[default]
frame_header=FE
frame_tail=FF
frame_length=4
channel_index=1
value_index=2
```

**特点：**
- 自动解析：从帧中提取通道号和值
- 无需配置每个通道：支持大量通道
- 适用场景：串口协议、结构化数据

**帧格式示例：**
```
[FE] [05] [01] [FF]  → DI通道5 = 1
[FE] [0C] [00] [FF]  → DI通道12 = 0
[FE] [64] [01] [FF]  → DI通道100 = 1
```

### 模式3: 混合模式（推荐）

同时使用两种模式，发挥各自优势：

```ini
[InputMapping]
; 重要命令使用显式映射（优先匹配）
0=START
1=STOP
2=EMERGENCY_STOP

; 其他通道使用帧解析
; 例如：收到 [FE][05][01][FF] 自动触发通道5
```

**处理流程：**
1. 收到数据
2. 先查找显式映射（InputMapping）
3. 如果匹配，触发对应通道
4. 如果不匹配，尝试帧解析
5. 如果解析成功，触发解析出的通道
6. 否则忽略数据

**优势：**
- **灵活性**：既有精确控制，又有自动处理
- **简化配置**：不需要配置所有通道
- **可扩展**：轻松支持100+通道
- **向后兼容**：不影响现有配置

## 使用场景示例

### 场景1: 纯ASCII命令系统

```ini
[default]
protocol=tcp://192.168.1.100:8080
data_format=ascii

[InputMapping]
0=CMD_START
1=CMD_STOP
2=CMD_RESET
3=CMD_PAUSE
```

不使用帧解析，所有输入都通过显式映射。

### 场景2: 纯串口帧协议

```ini
[default]
protocol=serial:///COM5?baudrate=115200
frame_header=FE
frame_tail=FF
frame_length=4
channel_index=1
value_index=2
```

不配置 InputMapping，所有数据通过帧解析。

### 场景3: 混合模式（推荐）

```ini
[default]
protocol=serial:///COM5?baudrate=115200
frame_header=FE
frame_tail=FF
frame_length=4
channel_index=1
value_index=2

[InputMapping]
; 紧急停止命令（优先级高）
0=EMERGENCY_STOP

; 特殊控制命令
10=0xFE0AAAFF
11=0xFE0BBBFF

; 其他通道 1-9, 12-255 通过帧自动解析
; 收到 [FE][05][01][FF] → 通道5
; 收到 [FE][64][01][FF] → 通道100
```

## 超时机制

配置输入通道超时时间：

```ini
input_timeout_ms=1000
```

**行为：**
- 通道被触发后开始计时
- 如果在超时时间内没有新数据，通道自动置0
- 适用于：按键检测、脉冲信号等

**示例：**
```
t=0ms:    收到 [FE][05][01][FF] → 通道5=1
t=500ms:  通道5 保持=1
t=1000ms: 超时，通道5 自动=0
```

## 完整配置示例

```ini
[default]
protocol=serial:///COM5?baudrate=115200
data_format=auto
write_wait_ms=60

; 帧格式
frame_header=FE
frame_tail=FF
frame_length=4
channel_index=1
value_index=2
input_timeout_ms=1000

[InputMapping]
; 显式映射（高优先级命令）
0=START
1=STOP
10=0xFEAAAAFF

; 通道 2-9, 11-255 使用帧解析

[OutputMapping]
0=CMD_OPEN
1=CMD_CLOSE
```

## 调试建议

启用详细日志后，可以看到数据处理流程：

```
[DeviceContext] Received: FE 05 01 FF
[DeviceContext] No explicit mapping found
[FrameProcessor] Frame extracted: FE 05 01 FF
[FrameProcessor] Parsed: channel=5, value=1
[DeviceContext] DI channel 5 = 1 (parsed from frame)
```

这样可以清楚看到是通过显式映射还是帧解析触发的通道。