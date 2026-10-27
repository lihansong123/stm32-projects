/* ============================================================================
 *  gpio_reg_led.c  ——  第 1 周动手：寄存器版 点灯 + 按键 + 开漏实验（STM32F103）
 * ----------------------------------------------------------------------------
 *  特点：**不依赖 stm32f10x.h / HAL**，自带最小寄存器定义。
 *        这样你会看懂 CMSIS 头文件到底在干什么（外设基址 + 结构体偏移），
 *        也方便直接放进去任何工程。
 *
 *  ⚠️ 三件事必须先做（否则代码再对也点不亮）：
 *    1. 打开你自己板子的**原理图**，确认 LED 接在哪个引脚、高电平亮还是低电平亮、
 *       有没有串限流电阻；确认按键接 GND 还是 VCC。
 *       下面的 LED_PIN / KEY_PIN 只是示例，请按你的板子改。
 *    2. 在本文件里改 LED_ACTIVE_HIGH（1 = 高电平亮，0 = 低电平亮）。
 *    3. 确认主频：本文件的 delay_ms 按 **72MHz** 计算。
 *       如果你的工程启动代码没配到 72MHz，延时会不准（这也正是"时钟自检实验"的意义）。
 *
 *  移植到 Keil 工程的两种方式：
 *    A) 直接用本文件：新建工程 → 把本 .c 加进去 → 编译下载（无需任何库）。
 *    B) 已有 stm32f10x.h 的工程：删掉下面"最小寄存器定义"整段，
 *       并在文件开头 #include "stm32f10x.h"，其余代码不用改
 *       （因为 RCC/GPIOB/SysTick 的名字与 CMSIS 一致，只需确认宏名对应）。
 *
 *  本周要做的三个实验（都在这一个工程里完成）：
 *    实验① 基础点灯：LED 按 500ms 闪烁（顺便当时钟自检）
 *    实验② 按键控制：按下常亮、松开常灭（20ms 软件去抖）
 *    实验③ 开漏实验：把 LED 改成开漏输出 → 观察现象 → 加外部上拉 → 恢复
 *
 *  编码：本文件是 UTF-8 with BOM（Keil/armcc 必须，否则中文注释会导致
 *        `missing closing quote` / `improperly terminated macro invocation`）。
 *        实测：armcc 在中文 Windows 下按 GBK 读源码，UTF-8 中文紧跟英文引号时引号会被吞。
 *        若 Keil 里中文显示乱码：Edit → Configuration → Encoding 选 UTF-8。
 * ==========================================================================*/

#include <stdint.h>

/* ==========================================================================
 *  最小寄存器定义（对应 CMSIS 头文件里做的事）
 * ==========================================================================*/

/* --- 外设基址（取自 F103 参考手册 RM0008 存储器映射） --- */
#define PERIPH_BASE        0x40000000u
#define APB2PERIPH_BASE    (PERIPH_BASE + 0x00010000u)   /* 0x40010000 */
#define AHBPERIPH_BASE     (PERIPH_BASE + 0x00020000u)   /* 0x40020000 */

#define GPIOA_BASE         (APB2PERIPH_BASE + 0x0800u)   /* 0x40010800 */
#define GPIOB_BASE         (APB2PERIPH_BASE + 0x0C00u)   /* 0x40010C00 */
#define GPIOC_BASE         (APB2PERIPH_BASE + 0x1000u)   /* 0x40011000 */
#define AFIO_BASE          (APB2PERIPH_BASE + 0x0000u)   /* 0x40010000 */

#define RCC_BASE           (AHBPERIPH_BASE + 0x1000u)    /* 0x40021000 */

/* --- GPIO 寄存器组（偏移量取自参考手册） --- */
typedef struct {
    volatile uint32_t CRL;    /* 0x00 端口配置低寄存器：PIN0~7  */
    volatile uint32_t CRH;    /* 0x04 端口配置高寄存器：PIN8~15 */
    volatile uint32_t IDR;    /* 0x08 输入数据寄存器（只读）    */
    volatile uint32_t ODR;    /* 0x0C 输出数据寄存器            */
    volatile uint32_t BSRR;   /* 0x10 位设置/清除寄存器（原子）  */
    volatile uint32_t BRR;    /* 0x14 位清除寄存器              */
    volatile uint32_t LCKR;   /* 0x18 配置锁定寄存器            */
} GPIO_TypeDef;

/* --- RCC 寄存器组（只列你要用的） --- */
typedef struct {
    volatile uint32_t CR;         /* 0x00 时钟控制（HSION/HSERDY/PLLON...） */
    volatile uint32_t CFGR;       /* 0x04 时钟配置（SW/SWS/HPRE/PLLMUL...） */
    volatile uint32_t CIR;        /* 0x08 */
    volatile uint32_t APB2RSTR;   /* 0x0C */
    volatile uint32_t APB1RSTR;   /* 0x10 */
    volatile uint32_t AHBENR;     /* 0x14 */
    volatile uint32_t APB2ENR;    /* 0x18 外设时钟使能（GPIO 时钟在这里！） */
    volatile uint32_t APB1ENR;    /* 0x1C */
    volatile uint32_t BDCR;       /* 0x20 */
    volatile uint32_t CSR;        /* 0x24 */
} RCC_TypeDef;

#define GPIOB   ((GPIO_TypeDef *) GPIOB_BASE)
#define RCC     ((RCC_TypeDef  *) RCC_BASE)

/* --- SysTick（内核外设，地址固定 0xE000E010） --- */
typedef struct {
    volatile uint32_t CTRL;   /* bit0 ENABLE, bit1 TICKINT, bit2 CLKSOURCE, bit16 COUNTFLAG */
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_TypeDef;
#define SysTick ((SysTick_TypeDef *) 0xE000E010u)

/* --- 位定义（名字与 CMSIS 一致，方便你迁移） --- */
#define RCC_APB2ENR_IOPBEN      (1u << 3)    /* GPIOB 时钟使能 */
#define RCC_APB2ENR_AFIOEN      (1u << 0)

/* ==========================================================================
 *  按你的板子改这三行
 * ==========================================================================*/
#define LED_PIN          0u      /* 示例：PB0。野火/BluePill 板常见 PC13，请查原理图 */
#define KEY_PIN          1u      /* 示例：PB1 */
#define LED_ACTIVE_HIGH  1       /* 1 = 高电平点亮；0 = 低电平点亮（PC13 那种通常是 0） */

/* ==========================================================================
 *  延时：用 SysTick 轮询，按 72MHz 算
 *  SysTick 是 24 位递减计数器，最大 0xFFFFFF ≈ 16.7M，72MHz 下最长约 233ms
 *  所以这里按 1ms 一段来数，安全且直观。
 * ==========================================================================*/
static void delay_ms(uint32_t ms)
{
    SysTick->LOAD = 72000u - 1u;      /* 72MHz / 1000 = 72000 个计数 = 1ms */
    SysTick->VAL  = 0u;
    SysTick->CTRL = 5u;               /* bit0 ENABLE=1, bit2 CLKSOURCE=1(HCLK)，不开中断 */
    while (ms--) {
        while ((SysTick->CTRL & (1u << 16)) == 0u) { }   /* 等 COUNTFLAG（读后自动清） */
    }
    SysTick->CTRL = 0u;               /* 用完关掉，别抢别人的 SysTick */
}

/* ==========================================================================
 *  GPIO 配置辅助函数：把 CNF/MODE 写进 CRL/CRH 对应的 4 位
 *  pin: 0~15 ; cnf: 0~3 ; mode: 0~3
 * ==========================================================================*/
static void gpio_config(GPIO_TypeDef *port, uint32_t pin, uint32_t cnf, uint32_t mode)
{
    volatile uint32_t *reg;
    uint32_t shift;

    if (pin < 8u) {
        reg   = &port->CRL;
        shift = pin * 4u;
    } else {
        reg   = &port->CRH;
        shift = (pin - 8u) * 4u;
    }
    *reg &= ~(0xFu << shift);                    /* 先清零这 4 位（读改写，必须清！） */
    *reg |=  ((cnf << 2) | mode) << shift;       /* 高 2 位 CNF，低 2 位 MODE */
}

/* ==========================================================================
 *  实验①：点灯闪烁（同时是"时钟自检"）
 * ==========================================================================*/
static void led_blink(void)
{
    for (;;) {
#if LED_ACTIVE_HIGH
        GPIOB->BSRR = (1u << LED_PIN);                 /* 置位 → 高电平 → 亮 */
        delay_ms(500);
        GPIOB->BSRR = (1u << (LED_PIN + 16u));         /* 高 16 位 = 复位 → 灭 */
        delay_ms(500);
#else
        GPIOB->BSRR = (1u << (LED_PIN + 16u));         /* 低电平亮：复位 = 亮 */
        delay_ms(500);
        GPIOB->BSRR = (1u << LED_PIN);
        delay_ms(500);
#endif
    }
}

/* ==========================================================================
 *  实验②：按键控制 LED（上拉输入 + 20ms 软件去抖）
 * ==========================================================================*/
static void key_control_led(void)
{
    for (;;) {
        if ((GPIOB->IDR & (1u << KEY_PIN)) == 0u) {     /* 上拉输入：按下 = 0 */
            delay_ms(20);                               /* 软件去抖（第 4 周换成状态机/定时器） */
            if ((GPIOB->IDR & (1u << KEY_PIN)) == 0u) { /* 20ms 后还是按下 → 确认 */
#if LED_ACTIVE_HIGH
                GPIOB->BSRR = (1u << LED_PIN);
#else
                GPIOB->BSRR = (1u << (LED_PIN + 16u));
#endif
            }
        } else {                                        /* 松开 → 灭 */
#if LED_ACTIVE_HIGH
            GPIOB->BSRR = (1u << (LED_PIN + 16u));
#else
            GPIOB->BSRR = (1u << LED_PIN);
#endif
        }
    }
}

/* ==========================================================================
 *  main
 * ==========================================================================*/
int main(void)
{
    /* ---- 1. 开 GPIOB 时钟（不开时钟，后面所有寄存器写入都无效！） ---- */
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    /* ---- 2. 配 LED 引脚：推挽输出 50MHz → CNF=00, MODE=11 ---- */
    gpio_config(GPIOB, LED_PIN, 0u, 3u);

    /* ---- 3. 配按键引脚：上拉输入 → CNF=10, MODE=00，并写 ODR=1 选上拉 ---- */
    gpio_config(GPIOB, KEY_PIN, 2u, 0u);
    GPIOB->ODR |= (1u << KEY_PIN);          /* ← 漏了这一行就变成"下拉输入"，按键永远读不到 0 */

    /* ---- 4. 初始灭灯 ---- */
#if LED_ACTIVE_HIGH
    GPIOB->BSRR = (1u << (LED_PIN + 16u));
#else
    GPIOB->BSRR = (1u << LED_PIN);
#endif

    /* ---- 5. 选一个实验跑（做完一个注释掉换下一个）---- */
    key_control_led();
    /* led_blink(); */

    /* ------------------------------------------------------------------
     *  实验③ 开漏实验（做完在笔记里写三行结论）：
     *    1) 把上面第 2 步改成：gpio_config(GPIOB, LED_PIN, 1u, 3u);   // CNF=01 开漏输出
     *    2) 保持 LED 外部**没有上拉电阻**，运行实验①。
     *       预期现象：________________________________（自己填）
     *       原因：____________________________________（用"高阻/只能拉低"解释）
     *    3) 在 LED 与 3.3V 之间加一个 4.7kΩ~10kΩ 上拉，再跑一次。
     *       预期现象：________________________________
     *    结论：I2C 的 SDA/SCL 为什么必须是开漏 + 上拉？

     *  额外实验④（时钟自检）：把工程里的 PLLMUL 改成 ×4（36MHz），
     *    重新下载，用秒表量 10 个闪烁周期。
     *    预期：耗时约为原来的 2 倍 → 说明延时确实跟着主频走。
     *    然后把 PLL 关掉、只用 HSI 8MHz：耗时约为 9 倍。改完记得改回来。
     * ------------------------------------------------------------------ */
    for (;;) { }
}

/* ==========================================================================
 *  自查清单（下载运行前逐条确认）
 *  [ ] 原理图确认了 LED / 按键引脚与有效电平
 *  [ ] LED_PIN / KEY_PIN / LED_ACTIVE_HIGH 已按自己板子修改
 *  [ ] RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; 这行在（没删）
 *  [ ] 按键那行 GPIOB->ODR |= (1u << KEY_PIN); 在（上拉必须写 ODR）
 *  [ ] 工程启动代码确实把主频配到了 72MHz（否则 delay_ms 不准）
 *  [ ] 没有动 PA13/PA14（SWDIO/SWCLK），否则会连不上下载器
 * ==========================================================================*/
