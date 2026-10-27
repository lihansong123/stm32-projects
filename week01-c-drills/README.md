# week01-c-drills —— 第 1 周 C 语言指针练习

纯主机端 C，不涉及 MCU。三个文件，各管一件事。

| 文件 | 干什么 | 跑完的判定 |
|---|---|---|
| `pointer_drills.c` | T1~T10 指针练习，**自带判分**（`PASS/FAIL`） | 全部 PASS，且每题注释里补上了"考点" |
| `memory_layout.c` | 打印各变量地址，验证 `.data`/`.bss`/`.rodata`/栈/堆 | 能把地址填进笔记里的内存四区图 |
| `gpio_reg_led.c` | STM32F103 寄存器版点灯 + 按键 + 开漏实验 | 在 Keil 里编译下载，现象符合预期 |

## 怎么跑前两个（需要一个主机 C 编译器）

本机当时没有 gcc，三种方式任选：

```bash
# A) w64devkit / MSYS2 / MinGW 拿到 gcc 后
gcc -std=c11 -Wall -Wextra -O0 -o drills pointer_drills.c && ./drills
gcc -std=c11 -Wall -Wextra -O0 -o layout memory_layout.c && ./layout
```

- **B) 在线编译器**：搜 onlinegdb / 菜鸟工具，粘贴源码点运行（浏览器能上网就行）。
- **C) 只用 Keil 查语法**：加进工程编译，0 error 即语法通过；但要看 PASS/FAIL 还是得 A 或 B。

> `memory_layout.c` 必须用 `-O0`，开优化后变量可能被优化掉，地址就"消失"了。

## gpio_reg_led.c 怎么用

1. 打开自己板子的**原理图**，确认 LED/按键的引脚与有效电平 → 改文件顶部 `LED_PIN` / `KEY_PIN` / `LED_ACTIVE_HIGH`。
2. Keil 新建工程 → 加入本 .c → 编译下载（**不需要** stm32f10x.h 或 HAL，文件里自带最小寄存器定义）。
3. 文件里写了 4 个实验：点灯闪烁 / 按键控制 / 开漏实验 / 时钟自检，做完一个注释掉换下一个。
4. 开漏实验的结论（三行）写进 `docs/` 里的记录，或直接补进本 README。

## 踩坑记录（自己补）

- 编码：三个 .c 都是 **UTF-8 with BOM**。armcc 在中文 Windows 下默认按 GBK 读源码，
  UTF-8 中文紧跟英文引号时会吞掉收尾引号，报 `missing closing quote`。实测加 BOM 后 0 错。
-