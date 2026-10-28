# 与 AI 大模型的对话完整记录

> 用途：记录本次作业中“AI 辅助学习”的完整过程，按要求以 Markdown 提交。
> 时间：2026-10-28
> 工具：DeepSeek Harness（编码智能体）+ Keil MDK-ARM 5.06 + STM32F103C8T6 实验板
> 说明：以下内容按对话顺序整理，保留了关键提问、AI 的回答要点、实际执行的命令与结果；
> 代码与报告均为在此基础上人工确认后落地。

---

## 0 起始需求

**我（学生）**：

> 图片是第 7 周作业：任务一用 uC/OS（以 uC/OS-II 为例）移植到 STM32F103，建 3 个任务：
> 第 1、2 个任务分别以 1 s、3 s 周期控制 LED 亮灭，第 3 个任务以 2 s 周期通过串口发送
> “hello uc/OS! 欢迎来到 RTOS 多任务环境！”，并记录详细移植过程。
> 任务二学习 I²C，用 STM32F103 完成基于 I²C 的 AHT20 温湿度采集，串口输出；解释软件 I²C 和硬件 I²C；
> 每 2 秒采集一次；在任务一基础上把第 3 个任务改成每 2 秒采集温湿度并发到上位机。
> 工程放在 `C:\Users\Administrator\Desktop\STM\work\7`，帮我完成。

**AI**：先确认了目录是空的，检查了本机已有的 Keil 安装、HAL 库、uC/OS-II 源码与前几周工程，
然后给出实施顺序：搭工程 → 移植内核 → 写 BSP/AHT20 → 编译到 0 Error/0 Warning → 画图写报告 → 打包提交。

> 实际动作：`Get-ChildItem` 检查 `work\7`；在 `Desktop\STM` 下找到 Keil（UV4.exe、ARMCC 5.06u5）；
> 在 `Desktop\STM\STM32\模块资料\...\uCOSII2.91` 找到 uC/OS-II V2.91 源码；
> 在 `work\6` 找到同芯片的 HAL 工程，复用 Drivers、启动文件与工程模板。

---

## 1 选 uC/OS-II 还是 uC/OS-III？

**我**：任务说明里写“以 uC/OS 为例”，参考链接里既有 uC/OS-II 也有 uC/OS-III，选哪个？

**AI**：

- 课程要求“至少 3 个任务 + 串口 + LED”，uC/OS-II 完全够用；
- uC/OS-III 源码更大、配置项更多，移植步骤反而更容易出错；
- 本机已有 **uC/OS-II V2.91** 的完整源码和 Cortex-M3 RealView 移植模板，可以直接用；
- 经典教材、正点原子/野火例程大多基于 uC/OS-II，报告里好写“移植过程”。

**结论**：选 **uC/OS-II V2.91**，`OS_TICKS_PER_SEC = 1000`（1 ms 节拍）。

---

## 2 工程应该基于 HAL 还是标准库？

**我**：参考链接里的例程有的是标准库，有的是 HAL，前几周作业用的是 CubeMX + HAL，能混吗？

**AI**：不要混。uC/OS-II 只依赖 CPU 移植层，和 HAL/标准库无关；但同一个工程里混两套库会重复定义、
中断向量也会冲突。建议沿用前几周 **CubeMX + HAL** 的工程骨架：HAL 负责时钟、GPIO、USART、I2C，
uC/OS-II 只接管 SysTick 和 PendSV。这样报告里的时钟树、引脚配置与前几周一致。

**落地**：复用 `work\6` 的 `Drivers/`（CMSIS + STM32F1xx HAL 1.1.10）、`startup_stm32f103xb.s`
和 Keil 工程模板，手写 `main.c`、`app.c`、BSP 与驱动。

---

## 3 uC/OS-II 源码放进来就报“找不到头文件”

**我**：把 `uCOSII2.91` 的 `Source`、`Ports` 拷进工程后，编译报：

```text
error: #5: cannot open source input file "uCOS-II\Source\ucos_ii.h"
```

**AI**：这份源码是从示例工程里拷出来的，`#include` 写死了带反斜杠的相对路径：

```c
#include <uCOS-II\Source\ucos_ii.h>
#include <uCOS-II\Ports\ARM-Cortex-M3\RealView\os_cpu.h>
```

换目录后当然找不到。两个办法：把工程目录结构做成和示例一样（不推荐），或者把所有源文件里的
路径改成 `<ucos_ii.h>`、`<os_cpu.h>`，再把 `Source`、`Ports` 加进 Include Path。

**落地**：批量替换 10 个 `.c` 文件与 `ucos_ii.h` 的 include；后续编译通过。

---

## 4 SysTick 和 HAL 时基打架怎么办？

**我**：HAL 的 `HAL_Delay()` 用 SysTick，uC/OS-II 的节拍也用 SysTick，会不会冲突？

**AI**：会，但可以共用。关键是两点：

1. **中断里两个都调用**：

```c
void SysTick_Handler(void)
{
    HAL_IncTick();              /* HAL 的 1 ms 计数 */
    OS_CPU_SysTickHandler();    /* OSIntEnter + OSTimeTick + OSIntExit */
}
```

2. **优先级**：`HAL_Init()` 默认把 SysTick 优先级设为 0（最高），而 uC/OS-II 要求节拍中断
   优先级最低（否则 `OSIntExit()` 可能在中断嵌套中提前调度）。在 `OS_CPU_SysTickInit()` 里
   把 SysTick 设为 15，`OSStartHighRdy` 会把 PendSV 设为 0xFF。

另外，任务里不要用 `HAL_Delay()` 做长延时（它会忙等、不让出 CPU），要改用 `OSTimeDly()`；
AHT20 的 80 ms 转换等待就用 `OSTimeDly(80)`。

---

## 5 PendSV 里到底要保存哪些寄存器？

**我**：Cortex-M3 异常自动压栈 R0–R3、R12、LR、PC、xPSR，为什么 PendSV 还要保存 R4–R11？

**AI**：因为自动压栈只覆盖“调用者保存”寄存器，R4–R11 是“被调用者保存”，异常返回不会自动恢复。
uC/OS-II 的上下文切换必须把 R4–R11 也压到任务栈，并把新的 PSP 存进 `OSTCBCur->OSTCBStkPtr`：

```asm
MRS   R0, PSP
SUBS  R0, R0, #0x20
STM   R0, {R4-R11}
LDR   R1, =OSTCBCur
LDR   R1, [R1]
STR   R0, [R1]
```

切换时再 `LDM` 恢复，`MSR PSP, R0`，`ORR LR, LR, #0x04` 保证异常返回使用 PSP。
`OSStartHighRdy` 还要先把 PSP 清 0（第一次切换不保存）、置 `OSRunning=TRUE`、触发 PendSV。

---

## 6 链接错误：Undefined symbol OSTaskReturnHook

**我**：所有文件都编译过了，链接报：

```text
Error: L6218E: Undefined symbol OSTaskReturnHook (referred from ucos_ii.o)
```

**AI**：`OSTaskReturnHook()` 是 uC/OS-II 2.91 新增的钩子（任务函数意外返回时调用），
2.86 的 `os_cpu_c.c` 里没有。补一个空实现即可：

```c
void OSTaskReturnHook(OS_TCB *ptcb)
{
    (void)ptcb;
}
```

补完后链接通过，`Program Size: Code=17988 RO-data=1132 RW-data=72 ZI-data=7048`，
**0 Error / 0 Warning**。

---

## 7 三个任务怎么划分？优先级怎么定？

**我**：LED1 1 s、LED2 3 s、串口 2 s，优先级该怎么排？

**AI**：原则是“周期越短、实时性要求越高，优先级越高”，同时要考虑阻塞行为：

| 任务 | 优先级 | 理由 |
| --- | --- | --- |
| 启动任务 | 3 | 只运行一次，创建完任务就删除自己 |
| AHT20 | 4 | 要保证 2 s 采样时刻，且 I²C 传输不能被 LED 拖太久 |
| LED1 | 5 | 1 s 翻转，实时性要求低于采样 |
| LED2 | 6 | 3 s 翻转，最不紧急 |

栈大小：AHT20 用 384 字（`vsnprintf` + HAL I2C 调用吃栈），启动任务 256 字，LED 各 128 字；
打开 `OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK`，统计任务会监控栈水位。

---

## 8 软件 I²C 和硬件 I²C 到底差在哪？

**我**：作业要求解释“软件 I²C”和“硬件 I²C”，还要读野火教材第 23 章，怎么讲清楚？

**AI**：一句话：**软件 I²C 用 GPIO 按位“手搓”时序，硬件 I²C 用芯片内部的 I²C 状态机**。

- 软件：把 SCL/SDA 配成开漏输出，CPU 逐位翻转；起始/停止/应答/采样都在代码里；速率由延时决定，
  本工程约 150–200 kHz；优点是任意引脚、易调试，缺点是占 CPU、时序受中断影响。
- 硬件：配置 I2C1 的时钟、地址模式、占空比，写 `DR` 后硬件自动产生 START/SCL/ACK/STOP；
  本工程 400 kHz；优点是 CPU 占用低、时序精确，缺点是引脚固定、总线挂死时不好恢复。

野火第 23 章用 EEPROM 举例：起始信号、7 位地址 + 读写位、ACK、字节传输、停止信号，
以及 EEPROM 的页写（跨页要分段）和 5 ms 写周期。把这一套时序对应到 AHT20 的“写 3 字节命令 +
等待 80 ms + 读 7 字节”，就很好理解。

**落地**：`aht20.c` 用 `AHT20_USE_SOFT_I2C` 宏切换传输层，上层测量流程共用；两种配置都编译通过。

---

## 9 AHT20 的命令与数据帧

**我**：AHT20 手册里命令、状态位、7 字节格式看得有点乱，帮我梳理。

**AI**：

- 7 位地址 **0x38**（写 0x70，读 0x71）；
- 上电等 ≥40 ms；读状态字节，bit3（0x08）是校准位，为 0 时写 `0xBE 0x08 0x00` 校准，延时 10 ms；
- 触发测量：写 `0xAC 0x33 0x00`，转换约 75–80 ms；bit7（0x80）是忙标志；
- 读 7 字节：Byte0 状态，Byte1–Byte3 为湿度 20 bit，Byte3 低 4 bit + Byte4 + Byte5 为温度 20 bit，Byte6 是 CRC；
- 换算：`RH = raw/2^20×100`，`T = raw/2^20×200−50`。

**我**：那代码里为什么用 `>>20` 和整数？

**AI**：Cortex-M3 上浮点 printf 会引入很大的库和栈开销；把结果换成 0.01 单位定点整数，
只做 64 位乘 + 右移即可：

```c
*hum_centi  = (raw_hum  * 10000u) >> 20;          /* 0.01 %RH */
*temp_centi = (raw_temp * 20000u) >> 20 - 5000;   /* 0.01 ℃  */
```

---

## 10 一个被 AI 自查出来的错误：温度系数写成 2000

**我**：帮我把换算公式再核对一遍。

**AI**：发现初版代码写的是 `raw_temp * 2000 >> 20 - 5000`，这是错的。推导：

```text
T = raw/2^20 × 200 − 50
T×100 = raw × 20000 / 2^20 − 5000
```

所以系数应该是 **20000**，不是 2000。用 `raw_temp = 394999` 验证：
`394999 × 20000 >> 20 = 7534`，`7534 − 5000 = 2534`，即 +25.34 ℃；若用 2000 会得到 −42.47 ℃，
明显不对。已修改驱动、报告、插图里的公式，并重新编译。

> 这件事说明：AI 给的代码也必须用数据手册的边界值回代验证，不能直接相信。

---

## 11 第 3 个任务怎么从“hello”改成“采集”？

**我**：任务一要求第 3 个任务发 hello，任务三又要求把它改成采集，两个都要交怎么办？

**AI**：用一个编译期宏保留两种形态：

```c
#define APP_TASK3_SENSOR   1   /* 0 = hello；1 = AHT20 */
```

`App_TaskAht20()` 里 `#if APP_TASK3_SENSOR` 走采集分支，否则每 2 s 打印问候语；
启动横幅也随宏变化。这样一份工程能复现两个阶段的实验现象。

**我**：直接 `OSTimeDly(2000)` 会不会漂移？

**AI**：会。读传感器约 80 ms，实际周期会变成 2080 ms。用 `OSTimeGet()` 记录循环起点，
读完后补足剩余时间：

```c
t0 = OSTimeGet();
/* ... 采集与输出 ... */
elapsed = OSTimeGet() - t0;
if (elapsed < 2000) OSTimeDly(2000 - elapsed);
```

这样时间戳间隔稳定在 2000–2002 ms。

---

## 12 串口输出交错怎么办？

**我**：启动任务和采集任务都调用 `BSP_Uart_Printf()`，会不会打架？

**AI**：会。`HAL_UART_Transmit` 内部有 `__HAL_LOCK`，并发调用会返回 `HAL_BUSY`，
或者两行字符交错。用 uC/OS-II 的互斥量包住“格式化 + 发送”：

```c
OSMutexPend(s_uart_lock, 0, &err);
vsnprintf(...);
HAL_UART_Transmit(...);
OSMutexPost(s_uart_lock);
```

互斥量带优先级继承，能避免优先级反转，比二值信号量更适合保护串口。

---

## 13 报告、插图与提交材料

**我**：报告要写“详细移植过程”，还要交 PDF、GitHub、博客、AI 对话记录，怎么组织？

**AI**：报告按“目的 → 环境 → 任务一移植（9 步 + 踩坑）→ 任务二 I²C/AHT20 → 任务三集成 →
编译资源 → 测试 → 总结 → 提交说明”组织；插图用 Pillow 生成（系统框图、任务时序、启动流程、
I²C 时序、软硬 I²C 对比、串口输出、存储占用），避免截图不清晰。Markdown 用 Chrome 无头模式
打印成 A4 PDF。提交 zip 里放源码、报告 MD/PDF、AI 记录、博客稿和 hex，排除 Keil 的 `.o/.crf` 等中间文件。

**落地**：

- `docs/tools/make_figs.py` 生成 7 张图；
- `docs/tools/md2html.py` + Chrome 生成完整 PDF；
- `docs/tools/make_zip.py` 打包 `dist/RTOS_AHT20_Task_632407030418.zip`；
- 工程编译 0 Error / 0 Warning，Flash 18.7 KB，RAM 7.0 KB。

---

## 14 实物联调：湿度到底在哪里看？

**我**：灯都正常亮，我在哪里看湿度？

**AI**：代码里湿度只通过 USART1 发到电脑，不在板子上显示；OLED 这次没有驱动，所以是黑的。
先检查电脑有没有串口——结果发现 DAP-Link 自带虚拟串口 **COM7**（USB Serial Device），
用 PowerShell 直接读 6 秒：

```powershell
$p = New-Object System.IO.Ports.SerialPort 'COM7',115200,'None',8,'One'
$p.Open()
while ($true) { $p.ReadLine() }
```

实际读到的数据：

```text
[01:56.259] #59  T=+24.84 C   H=60.30 %RH
[01:58.259] #60  T=+24.82 C   H=60.32 %RH
         CPU usage: 1 %
[02:00.259] #61  T=+24.79 C   H=60.31 %RH
```

又连续抓了 12 秒：

```text
[02:42.259] #82  T=+24.30 C   H=63.03 %RH
[02:44.259] #83  T=+24.27 C   H=62.67 %RH
[02:46.259] #84  T=+24.27 C   H=62.40 %RH
[02:48.259] #85  T=+24.24 C   H=62.34 %RH
         CPU usage: 1 %
[02:50.259] #86  T=+24.20 C   H=62.34 %RH
[02:52.259] #87  T=+24.21 C   H=62.42 %RH
[02:54.259] #88  T=+24.19 C   H=62.47 %RH
```

**AI**：这就同时验证了三件事：

1. 固件真的在跑（LED 正常 + 串口有数据）；
2. AHT20 的 I²C 通信正常，温湿度是真实读数（约 24.2 ℃ / 62.4 %RH）；
3. 任务三的 2 s 周期准确：相邻两行时间戳相差正好 **2000 ms**。

随后把报告里的“示例输出”全部替换成这段实测数据，插图也改成 COM7 的真实终端截图，
并在 README 里注明“DAP-Link 虚拟串口 COM7，不需要另接 CH340”。

> 经验：先看设备管理器有没有 COM 口，再看串口助手的波特率；DAP-Link 的虚拟串口只负责 UART，
> 下载仍走 SWD，两者互不影响。

---

## 15 复盘：AI 用得对不对？

**我**：这次 AI 主要帮了什么？哪些地方不能全信？

**AI**：

帮上忙的地方：

1. 快速定位本机已有的 uC/OS-II、HAL、Keil 资源，少走弯路；
2. 把移植拆成“数据类型/临界区、PendSV、SysTick”三件事，顺序清晰；
3. 解释 `OSTaskReturnHook`、SysTick 优先级、反斜杠 include 这些具体坑；
4. 生成插图、PDF 和打包脚本，节省排版时间。

不能全信的地方：

1. **温度系数 2000/20000** 的错误是回代数据手册才发现的，AI 生成的代码必须验证；
2. 串口数值必须用实物核对（本次已通过 COM7 实测，读数约 24.2 ℃ / 62.4 %RH）；
3. 硬件是否真的跑通，必须上电、看串口、量波形，AI 不能代替实物联调；
4. 版权与引用：uC/OS-II 源码来自 Micrium，报告里要注明，不能当成自己写的。

**结论**：AI 适合做“资料检索 + 方案对比 + 重复劳动”，核心的时序、公式、硬件现象必须自己核对。

---

## 附：本次实际执行的关键命令

```powershell
# 编译
& 'C:\Users\Administrator\Desktop\STM\UV4\UV4.exe' -r `
  'C:\Users\Administrator\Desktop\STM\work\7\MDK-ARM\RTOS_AHT20_Task.uvprojx' -j0 -o build.log

# 生成插图
python docs\tools\make_figs.py

# Markdown -> HTML -> PDF
python docs\tools\md2html.py
& 'C:\Program Files\Google\Chrome\Application\chrome.exe' --headless=new `
  --disable-gpu --no-pdf-header-footer `
  --print-to-pdf="docs\STM32_RTOS_AHT20实验报告.pdf" `
  "file:///C:/Users/Administrator/Desktop/STM/work/7/docs/STM32_RTOS_AHT20实验报告.html"

# 打包
python docs\tools\make_zip.py
```
