# RTOS_AHT20_Task —— STM32F103C8T6 + uC/OS-II + AHT20

第 7 周作业：把 **uC/OS-II V2.91** 移植到 STM32F103C8T6，建立 3 个任务；
并在 RTOS 上通过 **I²C** 每 2 s 采集一次 AHT20 温湿度，经 **USART1** 发送到上位机。

> 姓名 / 学号：蒋俊熙 / 632407030418

## 功能一览

| 任务 | 优先级 | 周期 | 内容 |
| --- | --- | --- | --- |
| `App_TaskStart` | 3 | 一次 | 打印启动信息、`OSStatInit()`、创建其余任务、删除自己 |
| `App_TaskAht20` | 4 | 2 s | 读 AHT20（I²C1 PB6/PB7），串口输出温度/湿度 |
| `App_TaskLed1` | 5 | 1 s | 翻转 PC13 板载 LED |
| `App_TaskLed2` | 6 | 3 s | 翻转 PB12 外接 LED |

- 系统节拍 **1 ms**（`OS_TICKS_PER_SEC = 1000`），PendSV 最低优先级、SysTick 优先级 15。
- 任务三的 2 s 周期用 `OSTimeGet()` 做补偿，长时间运行不漂移。
- 串口用 **uC/OS-II 互斥量**保护，避免启动任务与采集任务输出交错。
- AHT20 驱动同时支持 **硬件 I2C**（400 kHz）和 **软件模拟 I2C**（GPIO 开漏，约 350 kHz），
  宏 `AHT20_USE_SOFT_I2C` 一键切换。
- 任务一的“hello uc/OS!”形态保留：`APP_TASK3_SENSOR = 0` 时任务三每 2 s 发送问候语。

## 硬件连接

| 外设 | STM32F103C8T6 | 说明 |
| --- | --- | --- |
| AHT20 VCC / GND | 3V3 / GND | 3.3 V，勿接 5 V |
| AHT20 SCL | PB6 | I2C1_SCL，4.7 kΩ 上拉到 3V3 |
| AHT20 SDA | PB7 | I2C1_SDA，4.7 kΩ 上拉到 3V3 |
| USB-TTL RX | PA9 (USART1_TX) | 115200 8N1，交叉连接 |
| USB-TTL TX | PA10 (USART1_RX) | 与上一条交叉 |
| LED1 | PC13 | 板载 LED，低电平点亮 |
| LED2 | PB12 | 外接 LED（3V3 → 1 kΩ → LED → PB12），低电平点亮 |
| DAP-Link | 3V3/GND/PA13/PA14 | SWD 下载与调试 |

> 若只观察板载 LED，可把 `App_TaskLed2` 的引脚改成任意空闲 IO；不接 PB12 也不影响串口与传感器。

## 编译与下载

Keil MDK-ARM 5 打开 `MDK-ARM/RTOS_AHT20_Task.uvprojx`，F7 编译；命令行：

```powershell
$uv = 'C:\Users\Administrator\Desktop\STM\UV4\UV4.exe'
Start-Process $uv -ArgumentList @('-r', 'MDK-ARM\RTOS_AHT20_Task.uvprojx',
                                  '-j0', '-o', 'build.log') -Wait
```

当前结果：

```text
Program Size: Code=17988  RO-data=1132  RW-data=72  ZI-data=7048
0 Error(s), 0 Warning(s)
```

Flash ≈ 18.7 KB / 64 KB（29.2%），RAM ≈ 7.0 KB / 20 KB（34.8%）。
烧录文件：`MDK-ARM/RTOS_AHT20_Task/RTOS_AHT20_Task.hex`。

串口助手设为 **115200 8N1**，上电后可见：

```text
==================================================
  uC/OS-II V2.91   STM32F103C8T6 @ 72 MHz
  Tick: 1 ms        Tasks: LED1, LED2, AHT20
  LED1: 1 s (PC13)  LED2: 3 s (PB12)
  Task3: AHT20 over I2C1 (PB6/PB7), 2 s
--------------------------------------------------
  hello uc/OS! 欢迎来到RTOS多任务环境！
==================================================
[boot] AHT20 calibrated, sampling every 2 s
[00:00.128] #01  T=+25.34 C   H=48.20 %RH
[00:02.130] #02  T=+25.36 C   H=48.26 %RH
...
         CPU usage: 3 %
```

> 温度/湿度为示例值，实际读数随环境变化；时间戳由 `OSTimeGet()` 产生。

## 目录结构

```text
7/
├── Core/
│   ├── Inc/        main.h app_cfg.h os_cfg.h includes.h app.h
│   │               bsp_led.h bsp_uart.h aht20.h soft_i2c.h
│   └── Src/        main.c app.c bsp_led.c bsp_uart.c aht20.c soft_i2c.c
│                   stm32f1xx_it.c stm32f1xx_hal_msp.c system_stm32f1xx.c
├── Middlewares/uC-OS2/
│   ├── Source/     uC/OS-II V2.91 内核（ucos_ii.c + os_*.c）
│   └── Ports/ARM-Cortex-M3/RealView/  os_cpu.h os_cpu_a.asm os_cpu_c.c
├── Drivers/        CMSIS + STM32F1xx HAL 1.1.10
├── MDK-ARM/        RTOS_AHT20_Task.uvprojx + hex
├── docs/
│   ├── STM32_RTOS_AHT20实验报告.md / .pdf
│   ├── AI对话记录.md
│   ├── 博客-STM32F103-uCOSII-AHT20.md
│   ├── images/     报告插图
│   └── tools/      make_figs.py md2html.py make_zip.py
└── dist/           提交用 zip
```

## 移植要点

1. **`os_cpu.h`**：32 位数据类型、`OS_STK_GROWTH=1`、临界区方法 3（PRIMASK 保存/恢复）。
2. **`os_cpu_a.asm`**：`PendSV_Handler` 保存/恢复 R4–R11 与 PSP；
   `OSStartHighRdy` 把 PendSV 设为最低优先级并触发第一次切换。
3. **`os_cpu_c.c`**：`OSTaskStkInit()` 伪造异常栈帧；SysTick 产生 1 ms 节拍；
   补上 uC/OS-II 2.91 新增的 `OSTaskReturnHook()`（否则链接报 L6218E）。
4. **中断向量**：`PendSV_Handler` 由汇编强符号覆盖启动文件的弱定义；
   `SysTick_Handler` 同时调用 `HAL_IncTick()` 和 `OS_CPU_SysTickHandler()`，
   让 `HAL_Delay()` / HAL 超时在 RTOS 下继续可用。
5. **`os_cfg.h`**：只开任务、延时、互斥量、统计任务，关掉 flag/mbox/q/mem/sem/tmr，内核约 6 KB。

## 两个“坑”

- 仓库里的 uC/OS-II 源码来自示例工程，`#include <uCOS-II\Source\ucos_ii.h>` 这种反斜杠路径换目录后必然报错，要改成 `<ucos_ii.h>` / `<os_cpu.h>`。
- `HAL_Init()` 默认把 SysTick 优先级设为 0（最高），必须在 `OS_CPU_SysTickInit()` 里改成 15，否则中断嵌套时调度会出问题。

## 文档

- 实验报告：[`docs/STM32_RTOS_AHT20实验报告.pdf`](docs/STM32_RTOS_AHT20实验报告.pdf)
  （Markdown 源：[`.md`](docs/STM32_RTOS_AHT20实验报告.md)）
- AI 辅助学习记录：[`docs/AI对话记录.md`](docs/AI对话记录.md)
- 博客稿（可直接发 CSDN）：[`docs/博客-STM32F103-uCOSII-AHT20.md`](docs/博客-STM32F103-uCOSII-AHT20.md)
- 插图脚本：`docs/tools/make_figs.py`；报告转换：`docs/tools/md2html.py`

## 提交

`dist/RTOS_AHT20_Task_632407030418.zip` 包含工程源码、报告（MD + PDF）、AI 对话记录、博客稿与 hex，
可直接上传学习通。重新打包：

```powershell
python docs/tools/make_zip.py
```

## GitHub

仓库地址（已上传，可直接作为作业地址）：

```text
https://github.com/jkahwbdu/stm32-rtos-aht20-task
```

如需重新推送：

```bash
git init
git add .
git commit -m "feat: uC/OS-II port on STM32F103 + AHT20 over I2C"
git branch -M main
git remote add origin https://github.com/jkahwbdu/stm32-rtos-aht20-task.git
git push -u origin main
```

> 若网络访问 GitHub 报 schannel 证书错误，可临时使用
> `git -c http.sslBackend=openssl push -u origin main`。

## 说明

- 工程已在 Keil MDK 5.06 / ARMCC 5.06u5 下完整编译通过（0 Error / 0 Warning），
  硬件 I²C 与软件 I²C 两种配置均验证可编译。
- 实物联调步骤见报告第 7 节；若 AHT20 未接，串口会打印 `sensor error (code 1)`，程序不会死机。
- uC/OS-II 源码版权归 Micrium 所有，仅用于课程学习。
