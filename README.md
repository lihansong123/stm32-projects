# stm32-projects

STM32F103 工程集合。笔记在 [`embedded-notes`](https://github.com/)（另一个仓库）。

**规矩：一个工程一个目录，目录里的 README 写清"接线 + 现象 + 踩的坑"。** 面试官点开仓库先看 README，不看代码。

---

## 工程清单

| 目录 | 内容 | 关键点 | 状态 |
|---|---|---|---|
| `f103-template/` | 空工程模板（HSE 8M ×9 = 72MHz，Flash 2WS） | 以后每个新工程都从这复制，不再从零点 CubeMX | 待建 |
| `gpio-demo/` | 寄存器版 + HAL 版：LED 闪烁、按键上拉输入控制、开漏实验 | 解释推挽 vs 开漏、`BSRR` 原子性 | 待建 |
| `week01-c-drills/` | C 语言指针练习（T1~T10，主机编译运行） | 不含 MCU 代码，纯 C 基本功 | 已完成 |

> 目录都是干出来的：做完才建目录、才写 README，不要提前占位空文件夹。

## 每个工程目录里必须有

```
工程目录/
├── README.md            # 硬件接线、引脚表、现象、踩坑（必写）
├── <Keil 工程>.uvprojx  # 工程文件（要提交）
├── Core/ Inc/ Src/ ...  # 源码
└── docs/                # 截图：逻辑分析仪波形、CubeMX 时钟树配置、实物照片
```

`README.md` 模板：

```markdown
## 功能
## 硬件
| 引脚 | 用途 | 有效电平 | 备注 |
## 关键配置
（时钟/GPIO 模式/DMA/中断优先级，写清"为什么"）
## 现象
## 踩的坑
1. 现象 → 原因 → 解决
## 相关笔记
[第1周笔记](../embedded-notes/notes/week01/02-STM32-时钟树RCC与GPIO八种模式.md)
```

## 编译环境

- Keil MDK（`D:\Keil5`）+ ARMCC AC5 / ARMCLANG AC6
- STM32CubeMX（`D:\STM32CubeMX`）生成初始化代码
- 下载器：ST-Link / J-Link

## 提交规范

| 前缀 | 用途 | 例子 |
|---|---|---|
| `feat:` | 新功能/新外设跑通 | `feat(gpio): 按键上拉输入控制 LED` |
| `fix:` | 修 bug（提交信息里写清现象→原因） | `fix(rcc): 等 HSERDY 前漏开 HSE，导致切 PLL 后仍跑 8MHz` |
| `refactor:` | 分层重构 | `refactor: 拆成 bsp/driver/app 三层` |
| `docs:` | 只改 README/注释 | `docs: 补 gpio-demo 引脚表` |

**不要提交编译产物**（`Objects/`、`Listings/`、`*.axf`、`*.hex` 等已被 `.gitignore` 排除）。
例外：**发布版本固件**可以放 `release/` 并打 tag，面试时对方能直接下载烧录。

## 常用命令

```bash
git status -s
git add -A && git commit -m "feat(gpio): xxx"
git push
```