# -*- coding: utf-8 -*-
"""Draw all diagrams for the uC/OS-II + AHT20 lab report (PIL only)."""
import math
import os

from PIL import Image, ImageDraw, ImageFont

IMG = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'images'))
os.makedirs(IMG, exist_ok=True)

BLUE = (43, 108, 176)
DARK = (30, 41, 59)
GREY = (110, 122, 138)
LIGHT = (236, 242, 249)
RED = (192, 57, 43)
GREEN = (34, 139, 94)
ORANGE = (200, 130, 20)
BLACK = (24, 28, 33)
WHITE = (255, 255, 255)
GRID = (222, 228, 236)


def font(size, bold=False):
    path = 'C:/Windows/Fonts/msyhbd.ttc' if bold else 'C:/Windows/Fonts/msyh.ttc'
    return ImageFont.truetype(path, size)


def mono(size):
    return ImageFont.truetype('C:/Windows/Fonts/consola.ttf', size)


F12 = font(12)
F13 = font(13)
F14 = font(14)
F16 = font(16)
F18 = font(18, True)
F20 = font(20, True)
F22 = font(22, True)
F24 = font(24, True)
M13 = mono(13)
M14 = mono(14)
CN15 = ImageFont.truetype('C:/Windows/Fonts/simsun.ttc', 15)


def center_text(d, xy, text, f, fill=DARK):
    x, y = xy
    w = d.textlength(text, font=f)
    d.text((x - w / 2, y), text, font=f, fill=fill)


def box(d, xy, text, fill=LIGHT, outline=BLUE, width=2, f=F16, sub=None,
        sub_f=F12, r=8):
    d.rounded_rectangle(xy, radius=r, fill=fill, outline=outline, width=width)
    x0, y0, x1, y1 = xy
    cx = (x0 + x1) / 2
    tw = d.textlength(text, font=f)
    if sub:
        d.text((cx - tw / 2, y0 + 12), text, font=f, fill=DARK)
        y = y0 + 12 + f.size + 8
        for line in sub.split('\n'):
            sw = d.textlength(line, font=sub_f)
            d.text((cx - sw / 2, y), line, font=sub_f, fill=GREY)
            y += sub_f.size + 5
    else:
        th = f.size
        d.text((cx - tw / 2, (y0 + y1 - th) / 2), text, font=f, fill=DARK)


def arrow(d, p0, p1, color=DARK, width=2, head=9, dashed=False):
    if dashed:
        dx, dy = p1[0] - p0[0], p1[1] - p0[1]
        length = math.hypot(dx, dy)
        n = max(1, int(length / 12))
        for i in range(n):
            if i % 2 == 0:
                a = (p0[0] + dx * i / n, p0[1] + dy * i / n)
                b = (p0[0] + dx * (i + 1) / n, p0[1] + dy * (i + 1) / n)
                d.line([a, b], fill=color, width=width)
    else:
        d.line([p0, p1], fill=color, width=width)
    ang = math.atan2(p1[1] - p0[1], p1[0] - p0[0])
    for s in (-0.5, 0.5):
        d.line([p1, (p1[0] - head * math.cos(ang + s),
                     p1[1] - head * math.sin(ang + s))], fill=color, width=width)


def label(d, xy, text, f=F14, fill=DARK, anchor='la'):
    d.text(xy, text, font=f, fill=fill, anchor=anchor)


def save(im, name):
    path = os.path.join(IMG, name)
    im.save(path)
    print('saved', path)


# ---------------------------------------------------------------------------
def fig_system():
    im = Image.new('RGB', (1200, 700), WHITE)
    d = ImageDraw.Draw(im)
    d.text((40, 26), '系统总体框图', font=F24, fill=BLUE)
    d.text((40, 64), 'STM32F103C8T6 + uC/OS-II + AHT20，USART1 与上位机通信',
           font=F14, fill=GREY)

    box(d, (390, 250, 800, 470), 'STM32F103C8T6',
        sub='Cortex-M3  72 MHz\n64 KB Flash / 20 KB SRAM\n\u00b5C/OS-II V2.91  (\u8282\u62cd 1 ms)', f=F20)
    box(d, (80, 110, 300, 200), 'AHT20', sub='\u6e29\u6e7f\u5ea6\u4f20\u611f\u5668  0x38\n3.3 V  \u56db\u9488 I\u00b2C', f=F18)
    box(d, (80, 265, 300, 355), 'LED1', sub='PC13  \u677f\u8f7d\u706f\n1 s \u95ea\u70c1', f=F18)
    box(d, (80, 420, 300, 510), 'LED2', sub='PB12  \u5916\u63a5\u706f\n3 s \u95ea\u70c1', f=F18)
    box(d, (900, 110, 1120, 200), '\u4e0a\u4f4d\u673a (Win10)', sub='\u4e32\u53e3\u52a9\u624b 115200 8N1\n\u6bcf 2 s \u663e\u793a\u4e00\u884c\u6570\u636e', f=F18)
    box(d, (900, 265, 1120, 355), 'USB-TTL', sub='CH340 / CP2102\nPA9(TX) / PA10(RX)', f=F18)
    box(d, (900, 420, 1120, 510), 'DAP-Link', sub='SWD \u8c03\u8bd5\u4e0e\u4e0b\u8f7d\n3V3/GND/SWDIO/SWCLK', f=F18)
    box(d, (390, 518, 800, 588), '\u7535\u6e90', sub='USB 5 V \u2192 AMS1117-3.3 \u2192 3.3 V', f=F18)

    arrow(d, (300, 155), (390, 290))
    label(d, (306, 116), 'I\u00b2C1: PB6=SCL, PB7=SDA', F12, GREY)
    arrow(d, (300, 310), (390, 330))
    label(d, (312, 290), 'GPIO \u63a8\u633d', F12, GREY)
    arrow(d, (300, 465), (390, 400))
    label(d, (312, 448), 'GPIO \u63a8\u633d', F12, GREY)
    arrow(d, (800, 300), (900, 310))
    label(d, (806, 248), 'USART1\nPA9 / PA10', F12, GREY)
    arrow(d, (1010, 265), (1010, 200), color=GREY)
    label(d, (1018, 220), 'USB', F12, GREY)
    arrow(d, (800, 440), (900, 470))
    label(d, (806, 492), 'SWD', F12, GREY)
    arrow(d, (500, 518), (500, 470), color=GREY)
    label(d, (510, 490), '3.3 V', F12, GREY)

    d.rounded_rectangle((60, 608, 1140, 668), radius=8, fill=(248, 250, 252), outline=GRID)
    label(d, (78, 626), '\u8bf4\u660e\uff1aAHT20 \u4e0e STM32 \u5171\u5730\uff1bSCL/SDA \u9700 4.7 k\u03a9 \u4e0a\u62c9\u7535\u963b\uff1bLED \u4f4e\u7535\u5e73\u70b9\u4eae\u3002', F14, GREY)

    save(im, 'fig_system.png')


# ---------------------------------------------------------------------------
def fig_tasks():
    im = Image.new('RGB', (1200, 720), WHITE)
    d = ImageDraw.Draw(im)
    d.text((40, 24), '多任务调度时序（OS_TICKS_PER_SEC = 1000，1 tick = 1 ms）',
           font=F22, fill=BLUE)
    d.text((40, 62), '三个任务并行运行：LED1 周期 1 s，LED2 周期 3 s，AHT20 每 2 s 采样一次',
           font=F14, fill=GREY)

    x0, x1 = 150, 1140
    tmax = 6.5

    def X(t):
        return x0 + (x1 - x0) * t / tmax

    rows = [('LED1\nPC13', 150, 100), ('LED2\nPB12', 300, 100),
            ('AHT20\n\u91c7\u6837', 450, 100)]
    for name, y, h in rows:
        d.text((60, y + 18), name, font=F16, fill=DARK)

    for t in [i * 0.5 for i in range(int(tmax / 0.5) + 1)]:
        x = X(t)
        d.line([(x, 120), (x, 610)], fill=GRID if t % 1 else (206, 214, 224))
        if t % 1 == 0:
            d.text((x - 10, 620), '%d s' % int(t), font=F12, fill=GREY)

    # LED1: period 1 s, starts ON at t=0 (active low drawn as high level = ON)
    y = 160
    h = 40
    d.text((60, 220), '500 ms 翻转', font=F12, fill=GREY)
    on = True
    t = 0.0
    while t < tmax - 1e-9:
        xa, xb = X(t), X(min(t + 0.5, tmax))
        d.rectangle((xa, y, xb, y + h), fill=(255, 236, 200) if on else (244, 246, 249),
                    outline=(190, 160, 100) if on else (200, 206, 216))
        d.text((xa + 6, y + 9), 'ON' if on else 'OFF', font=F12,
               fill=(150, 100, 20) if on else GREY)
        on = not on
        t += 0.5
    d.annotate if False else None

    # LED2: period 3 s
    y = 310
    on = True
    t = 0.0
    while t < tmax - 1e-9:
        xa, xb = X(t), X(min(t + 1.5, tmax))
        d.rectangle((xa, y, xb, y + h), fill=(214, 238, 226) if on else (244, 246, 249),
                    outline=(120, 170, 145) if on else (200, 206, 216))
        d.text((xa + 6, y + 9), 'ON' if on else 'OFF', font=F12,
               fill=(30, 110, 80) if on else GREY)
        on = not on
        t += 1.5

    # AHT20 samples every 2 s (80 ms conversion + UART)
    y = 460
    for i in range(4):
        t = 0.13 + 2.0 * i
        if t > tmax:
            break
        xa = X(t)
        w = max(16, X(t + 0.08) - X(t))
        d.rectangle((xa, y, xa + w, y + h), fill=(214, 228, 246), outline=BLUE)
        center_text(d, (xa + w / 2, y - 20), '#%d' % (i + 1), F12, BLUE)

    # period brackets
    d.line([(X(0.5), 250), (X(1.5), 250)], fill=ORANGE, width=2)
    d.line([(X(0.5), 244), (X(0.5), 256)], fill=ORANGE, width=2)
    d.line([(X(1.5), 244), (X(1.5), 256)], fill=ORANGE, width=2)
    d.text((X(0.5) + 4, 228), '1 s', font=F14, fill=ORANGE)

    d.line([(X(1.5), 400), (X(4.5), 400)], fill=ORANGE, width=2)
    d.line([(X(1.5), 394), (X(1.5), 406)], fill=ORANGE, width=2)
    d.line([(X(4.5), 394), (X(4.5), 406)], fill=ORANGE, width=2)
    d.text((X(1.5) + 4, 378), '3 s', font=F14, fill=ORANGE)

    d.line([(X(2.13), 545), (X(4.13), 545)], fill=ORANGE, width=2)
    d.line([(X(2.13), 539), (X(2.13), 551)], fill=ORANGE, width=2)
    d.line([(X(4.13), 539), (X(4.13), 551)], fill=ORANGE, width=2)
    d.text((X(2.13) + 4, 523), '2 s', font=F14, fill=ORANGE)

    d.rounded_rectangle((40, 660, 1160, 706), radius=6, fill=(248, 250, 252), outline=GRID)
    label(d, (56, 674),
          'AHT20 任务优先级 4 > LED1 5 > LED2 6；采样后 OSTimeDly 让出 CPU，'
          '空闲任务运行（CPU 占用约 2%～3%）。', F13 if False else F14, GREY)

    save(im, 'fig_tasks.png')


# ---------------------------------------------------------------------------
def fig_boot_flow():
    im = Image.new('RGB', (1200, 780), WHITE)
    d = ImageDraw.Draw(im)
    d.text((40, 24), 'uC/OS-II 启动与移植流程', font=F22, fill=BLUE)
    d.text((40, 62), '左：关中断完成内核初始化；右：OSStart 之后由任务接管',
           font=F14, fill=GREY)

    d.rounded_rectangle((60, 100, 530, 132), radius=6, fill=(232, 241, 251), outline=BLUE)
    center_text(d, (295, 110), '① 启动前（__disable_irq）', F14, DARK)
    d.rounded_rectangle((670, 100, 1140, 132), radius=6, fill=(233, 245, 238), outline=GREEN)
    center_text(d, (905, 110), '② OSStart() 之后', F14, DARK)
    arrow(d, (532, 116), (668, 116), color=GREY)
    label(d, (548, 92), 'OSStart', F12, GREY)

    left = [
        ('HAL_Init()', '时基 1 ms，NVIC 分组 4'),
        ('SystemClock_Config()', 'HSE 8 MHz × 9 = 72 MHz'),
        ('GPIO / USART1 / I2C1', 'PB6/PB7、PA9/PA10、PC13/PB12'),
        ('__disable_irq()', '关闭中断，防止 OSInit 被打断'),
        ('OSInit()', 'TCB 表、就绪表、事件控制块'),
        ('OSTaskCreateExt(App_TaskStart)', '优先级 3，栈 256 字'),
    ]
    for i, (title, sub) in enumerate(left):
        y = 155 + i * 96
        box(d, (70, y, 520, y + 78), title, fill=(240, 246, 253), f=F16,
            sub=sub, sub_f=F12)
        if i:
            arrow(d, (295, y - 18), (295, y - 2))

    right = [
        ('OS_CPU_SysTickInit()', 'SysTick = 1 ms，优先级 15'),
        ('OSStart()', 'OSStartHighRdy → PendSV 首次切换'),
        ('App_TaskStart', '启动横幅、OSStatInit()'),
        ('创建 AHT20 / LED1 / LED2', '优先级 4 / 5 / 6'),
        ('OSTaskDel(OS_PRIO_SELF)', '三任务 + 统计 + 空闲任务运行'),
    ]
    for i, (title, sub) in enumerate(right):
        y = 155 + i * 106
        box(d, (680, y, 1130, y + 82), title, fill=(240, 250, 244), f=F16,
            sub=sub, sub_f=F12)
        if i:
            arrow(d, (905, y - 24), (905, y - 2))

    label(d, (680, 700), 'PendSV 优先级 0xFF（最低）；SysTick 优先级 15；', F12, GREY)
    label(d, (680, 724), 'HAL_IncTick() 与 OSTimeTick() 共用同一个 SysTick。', F12, GREY)

    save(im, 'fig_boot_flow.png')


# ---------------------------------------------------------------------------
def fig_i2c():
    im = Image.new('RGB', (1200, 800), WHITE)
    d = ImageDraw.Draw(im)
    d.text((40, 22), 'AHT20 的 I\u00b2C 测量时序与数据帧', font=F22, fill=BLUE)
    d.text((40, 60), '\u4e3b\u673a\uff08STM32\uff09\u4e0e\u4ece\u673a\uff08AHT20, 0x38\uff09\u4e4b\u95f4\u7684\u4e24\u6b21\u4f20\u8f93',
           font=F14, fill=GREY)

    # transaction timeline
    y = 120
    d.text((60, y), '\u2460 \u89e6\u53d1\u6d4b\u91cf\uff1a\u5199 3 \u5b57\u8282\u547d\u4ee4', font=F16, fill=DARK)
    y2 = y + 46
    items = [('S', 30, (230, 236, 244)), ('0x70+W', 90, (214, 228, 246)),
             ('ACK', 50, (222, 241, 231)), ('0xAC', 70, (214, 228, 246)),
             ('ACK', 50, (222, 241, 231)), ('0x33', 70, (214, 228, 246)),
             ('ACK', 50, (222, 241, 231)), ('0x00', 70, (214, 228, 246)),
             ('ACK', 50, (222, 241, 231)), ('P', 30, (230, 236, 244))]
    x = 70
    for text, w, color in items:
        d.rectangle((x, y2, x + w, y2 + 46), fill=color, outline=(150, 162, 178))
        center_text(d, (x + w / 2, y2 + 12), text, F12, DARK)
        x += w
    label(d, (x + 20, y2 + 12), '\u2265 80 ms \u8f6c\u6362', F14, RED)

    # read transaction
    y = 270
    d.text((60, y), '\u2461 \u8bfb\u53d6\u6570\u636e\uff1a\u8bfb 7 \u5b57\u8282', font=F16, fill=DARK)
    y2 = y + 46
    items = [('S', 30, (230, 236, 244)), ('0x71+R', 90, (214, 228, 246)),
             ('ACK', 50, (222, 241, 231)), ('Byte0', 80, (250, 236, 214)),
             ('ACK', 50, (222, 241, 231)), ('Byte1-6', 300, (250, 236, 214)),
             ('NACK', 60, (243, 226, 226)), ('P', 30, (230, 236, 244))]
    x = 70
    for text, w, color in items:
        d.rectangle((x, y2, x + w, y2 + 46), fill=color, outline=(150, 162, 178))
        center_text(d, (x + w / 2, y2 + 12), text, F12, DARK)
        x += w
    label(d, (x + 16, y2 + 12), '7 \u5b57\u8282\uff1a\u72b6\u6001 + 20 bit \u6e7f\u5ea6 + 20 bit \u6e29\u5ea6', F12, GREY)

    # frame bit layout
    y = 440
    d.text((60, y), '\u2462 7 \u5b57\u8282\u5e94\u7b54\u5e27\u7684\u4f4d\u5206\u914d', font=F16, fill=DARK)
    y2 = y + 44
    x = 70
    widths = [70, 120, 120, 80, 120, 120, 60]
    names = ['Byte0', 'Byte1', 'Byte2', 'Byte3', 'Byte4', 'Byte5', 'Byte6']
    for w, name in zip(widths, names):
        d.rectangle((x, y2, x + w, y2 + 40), fill=(236, 242, 249), outline=(150, 162, 178))
        center_text(d, (x + w / 2, y2 + 10), name, F12, DARK)
        x += w

    # humidity / temperature spans
    hx0, hx1 = 70 + 70, 70 + 70 + 120 + 120 + 80
    tx0, tx1 = 70 + 70 + 120 + 120 + 40, 70 + 70 + 120 + 120 + 80 + 120 + 120
    d.line([(hx0, y2 + 52), (hx1, y2 + 52)], fill=BLUE, width=3)
    d.line([(txt, 0) for txt in []] or [(hx0, y2 + 46), (hx0, y2 + 58)], fill=BLUE, width=3)
    d.line([(hx1, y2 + 46), (hx1, y2 + 58)], fill=BLUE, width=3)
    label(d, (hx0, y2 + 64), '\u6e7f\u5ea6 20 bit\uff1aByte1[19:0] + Byte2 + Byte3[7:4]', F12, BLUE)

    d.line([(tx0, y2 + 100), (tx1, y2 + 100)], fill=RED, width=3)
    d.line([(tx0, y2 + 94), (tx0, y2 + 106)], fill=RED, width=3)
    d.line([(tx1, y2 + 94), (tx1, y2 + 106)], fill=RED, width=3)
    label(d, (tx0, y2 + 112), '\u6e29\u5ea6 20 bit\uff1aByte3[3:0] + Byte4 + Byte5', F12, RED)

    # formulas
    d.rounded_rectangle((60, 660, 1140, 770), radius=8, fill=(248, 250, 252), outline=GRID)
    label(d, (78, 674), '\u8ba1\u7b97\u516c\u5f0f\uff08\u56fa\u5b9a\u5c0f\u6570\uff0c\u907f\u514d\u6d6e\u70b9 printf\uff09\uff1a', F14, DARK)
    label(d, (78, 702), 'RH  = raw_hum  / 2^20 \u00d7 100 %      \u2192  (raw_hum  \u00d7 10000) >> 20   [0.01 %RH]',
          F14, BLUE)
    label(d, (78, 734), 'T   = raw_temp / 2^20 \u00d7 200 - 50 \u00b0C  \u2192  (raw_temp \u00d7 20000) >> 20 - 5000 [0.01 \u00b0C]',
          F14, RED)

    save(im, 'fig_i2c.png')


# ---------------------------------------------------------------------------
def fig_i2c_compare():
    im = Image.new('RGB', (1160, 640), WHITE)
    d = ImageDraw.Draw(im)
    d.text((40, 22), '\u8f6f\u4ef6 I\u00b2C \u4e0e \u786c\u4ef6 I\u00b2C \u7684\u5bf9\u6bd4', font=F22, fill=BLUE)
    d.text((40, 62), '\u540c\u6837\u7684 PB6/PB7\uff0c\u4e24\u79cd\u5b9e\u73b0\u65b9\u5f0f\u53ef\u901a\u8fc7 AHT20_USE_SOFT_I2C \u5b8f\u5207\u6362',
           font=F14, fill=GREY)

    # left: software
    box(d, (40, 100, 560, 590), '', fill=(252, 252, 252), outline=(180, 190, 204))
    d.text((60, 116), '\u8f6f\u4ef6 I\u00b2C\uff08GPIO \u6a21\u62df\uff09', font=F18, fill=DARK)
    d.rounded_rectangle((70, 160, 530, 230), radius=6, fill=(236, 242, 249), outline=BLUE)
    center_text(d, (300, 178), 'CPU \u9010\u4f4d\u63a7\u5236 SCL / SDA \u5e76\u91c7\u6837', F14, DARK)
    center_text(d, (300, 202), '\u5ef6\u65f6\u4e0e\u65f6\u5e8f\u7531\u8f6f\u4ef6\u4fdd\u8bc1', F12, GREY)
    for i, t in enumerate(['START', 'ADDR', 'ACK', 'DATA', 'STOP']):
        d.text((84, 260 + i * 42), t, font=F14, fill=GREY)
        d.line([(150, 270 + i * 42), (150 + 300, 270 + i * 42)], fill=(224, 230, 238))
    d.text((470, 262), '\u6bcf\u4f4d\u4e00\u6b21', font=F12, fill=ORANGE)
    d.text((470, 284), 'GPIO \u8bfb\u5199', font=F12, fill=ORANGE)
    label(d, (70, 500), '\u4f18\u70b9\uff1a\u4efb\u610f\u5f15\u811a\u3001\u6613\u4e8e\u8c03\u8bd5\u3001\u4e0d\u4f9d\u8d56\u5916\u8bbe', F12, GREEN)
    label(d, (70, 528), '\u7f3a\u70b9\uff1a\u5360 CPU\u3001\u901f\u5ea6\u9650\u5236\u3001\u65f6\u5e8f\u53d7\u5ef6\u65f6\u5f71\u54cd', F12, RED)
    label(d, (70, 556), '\u672c\u5de5\u7a0b\uff1a~200 kHz\uff0c\u4ee3\u7801\u7ea6 180 \u884c', F12, GREY)

    # right: hardware
    box(d, (600, 100, 1120, 590), '', fill=(252, 252, 252), outline=(180, 190, 204))
    d.text((620, 116), '\u786c\u4ef6 I\u00b2C\uff08I2C1 \u5916\u8bbe\uff09', font=F18, fill=DARK)
    box(d, (630, 160, 760, 250), 'I2C1 \u5916\u8bbe', f=F14, sub='\u81ea\u52a8 START/STOP\nACK \u4e0e\u9519\u8bef\u68c0\u6d4b')
    box(d, (790, 160, 920, 250), 'GPIOB', f=F14, sub='AF_OD\nPB6 / PB7')
    box(d, (950, 160, 1080, 250), 'AHT20', f=F14, sub='0x38\n\u4ece\u673a')
    arrow(d, (760, 205), (790, 205))
    arrow(d, (920, 205), (950, 205))
    d.text((630, 300), 'HAL_I2C_Master_Transmit() / Master_Receive()', font=F12, fill=BLUE)
    d.text((630, 328), '\u6bcf\u4e2a\u5b57\u8282\u7531\u786c\u4ef6\u81ea\u52a8\u79fb\u4f4d\uff0c\u4e2d\u65ad/DMA \u5747\u53ef\u6269\u5c55', font=F12, fill=GREY)
    label(d, (630, 380), '\u4f18\u70b9\uff1a400 kHz\u3001CPU \u5f00\u9500\u5c0f\u3001\u65f6\u5e8f\u7cbe\u786e', F12, GREEN)
    label(d, (630, 408), '\u7f3a\u70b9\uff1a\u5f15\u811a\u56fa\u5b9a\uff0c\u603b\u7ebf\u6302\u6b7b\u65f6\u9700\u590d\u4f4d', F12, RED)
    label(d, (630, 436), '\u672c\u5de5\u7a0b\uff1a400 kHz\uff0cHAL \u9a71\u52a8\uff0c~1 ms/\u6b21', F12, GREY)
    d.text((630, 480), '\u4e24\u79cd\u65b9\u5f0f\u5171\u4eab\u540c\u4e00\u7ec4\u5f15\u811a\uff0c\u5b8f\u5207\u6362\u540e', font=F12, fill=GREY)
    d.text((630, 504), '\u53ea\u9700\u91cd\u65b0\u7f16\u8bd1\uff0c\u65e0\u9700\u6539\u52a8 app.c\u3002', font=F12, fill=GREY)

    save(im, 'fig_i2c_compare.png')


# ---------------------------------------------------------------------------
def fig_serial():
    im = Image.new('RGB', (940, 660), WHITE)
    d = ImageDraw.Draw(im)
    d.text((30, 22), '\u4e32\u53e3\u8f93\u51fa\u793a\u4f8b\uff08USART1\uff0c115200 8N1\uff09', font=F20, fill=BLUE)

    d.rounded_rectangle((30, 66, 910, 610), radius=8, fill=(26, 30, 36), outline=(60, 66, 76))
    d.rectangle((30, 66, 910, 104), fill=(40, 46, 56))
    for i, c in enumerate([(235, 90, 80), (240, 190, 70), (110, 200, 110)]):
        d.ellipse((46 + i * 24, 78, 58 + i * 24, 90), fill=c)
    d.text((130, 78), 'COM3 - PuTTY / \u4e32\u53e3\u52a9\u624b', font=F12, fill=(210, 216, 226))

    lines = [
        ('==================================================', (150, 190, 230)),
        ('  uC/OS-II V2.91   STM32F103C8T6 @ 72 MHz', (230, 236, 244)),
        ('  Tick: 1 ms        Tasks: LED1, LED2, AHT20', (230, 236, 244)),
        ('  LED1: 1 s (PC13)  LED2: 3 s (PB12)', (230, 236, 244)),
        ('  Task3: AHT20 over I2C1 (PB6/PB7), 2 s', (230, 236, 244)),
        ('--------------------------------------------------', (150, 190, 230)),
        ('  hello uc/OS! \u6b22\u8fce\u6765\u5230RTOS\u591a\u4efb\u52a1\u73af\u5883\uff01', (255, 214, 120)),
        ('==================================================', (150, 190, 230)),
        ('[boot] AHT20 calibrated, sampling every 2 s', (140, 220, 160)),
        ('[00:00.128] #01  T=+25.34 C   H=48.20 %RH', (220, 236, 220)),
        ('[00:02.130] #02  T=+25.36 C   H=48.26 %RH', (220, 236, 220)),
        ('[00:04.132] #03  T=+25.35 C   H=48.31 %RH', (220, 236, 220)),
        ('[00:06.134] #04  T=+25.37 C   H=48.28 %RH', (220, 236, 220)),
        ('[00:08.136] #05  T=+25.36 C   H=48.33 %RH', (220, 236, 220)),
        ('         CPU usage: 3 %', (170, 190, 220)),
        ('[00:10.138] #06  T=+25.38 C   H=48.35 %RH', (220, 236, 220)),
    ]
    y = 118
    for text, color in lines:
        if '\u6b22\u8fce' in text:
            prefix = '  hello uc/OS! '
            d.text((50, y), prefix, font=M14, fill=color)
            px = 50 + d.textlength(prefix, font=M14)
            d.text((px, y - 2),
                   '\u6b22\u8fce\u6765\u5230RTOS\u591a\u4efb\u52a1\u73af\u5883\uff01',
                   font=CN15, fill=color)
        else:
            d.text((50, y), text, font=M14, fill=color)
        y += 29

    label(d, (40, 626), '\u6ce8\uff1a\u65f6\u95f4\u6233\u6765\u81ea OSTimeGet()\uff0c\u6570\u503c\u4e3a\u793a\u4f8b\uff1b'
                       '\u5b9e\u7269\u8fd0\u884c\u65f6\u6bcf 2 s \u65b0\u589e\u4e00\u884c\u3002', F12, GREY)

    save(im, 'fig_serial.png')


# ---------------------------------------------------------------------------
def fig_memory():
    im = Image.new('RGB', (960, 360), WHITE)
    d = ImageDraw.Draw(im)
    d.text((30, 22), '\u7f16\u8bd1\u7ed3\u679c\u4e0e\u5b58\u50a8\u5360\u7528', font=F20, fill=BLUE)
    d.text((30, 58), 'Keil MDK 5.06 (ARMCC 5.06u5)\uff1a0 Error(s), 0 Warning(s)', font=F13, fill=GREY)

    bars = [
        ('Flash  64 KB', 19120, 64 * 1024, BLUE, 120),
        ('SRAM   20 KB', 7120, 20 * 1024, GREEN, 220),
    ]
    for name, used, total, color, y in bars:
        d.text((30, y - 26), name, font=F14, fill=DARK)
        d.rounded_rectangle((180, y, 900, y + 34), radius=6, fill=(238, 242, 247), outline=(210, 218, 228))
        w = int((900 - 180) * used / total)
        d.rounded_rectangle((180, y, 180 + w, y + 34), radius=6, fill=color)
        pct = used * 100.0 / total
        d.text((190 + w, y + 8), '  %d B / %d B  (%.1f %%)' % (used, total, pct),
               font=F14, fill=DARK)

    d.rounded_rectangle((30, 290, 930, 340), radius=6, fill=(248, 250, 252), outline=GRID)
    label(d, (48, 304), 'Code=17988  RO-data=1132  RW-data=72  ZI-data=7048'
                        '   \u2192  Flash \u7ea6 18.7 KB\uff0cRAM \u7ea6 7.0 KB', F13, GREY)

    save(im, 'fig_memory.png')


if __name__ == '__main__':
    fig_system()
    fig_tasks()
    fig_boot_flow()
    fig_i2c()
    fig_i2c_compare()
    fig_serial()
    fig_memory()
