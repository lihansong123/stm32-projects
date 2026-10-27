/* ============================================================================
 *  memory_layout.c  ——  第 1 周实验：亲眼看见"内存四区"
 * ----------------------------------------------------------------------------
 *  目的：把每个变量的地址打印出来，填进笔记里的四区图，验证：
 *        .data / .bss / .rodata / 栈 / 堆 各自所在的位置区间。
 *
 *  跑法（主机编译器，见 pointer_drills.c 顶部说明；在线编译器也可）：
 *        gcc -std=c11 -Wall -Wextra -O0 -o layout memory_layout.c && ./layout
 *        ⚠️ 一定要用 -O0：开优化后编译器可能把变量优化掉，地址就"消失"了。
 *
 *  注意：PC 上的地址分布（尤其栈和堆的上下关系）与 MCU 不同，
 *        但"哪一组在一起、哪一组分开"的结论是一致的。
 *
 *  编码：本文件是 UTF-8 with BOM。
 *        Keil/armcc 在中文 Windows 下默认按 GBK 读源码，UTF-8 中文紧跟英文引号时会吞掉引号，
 *        所以含中文的 .c 文件统一存 "UTF-8 with BOM"。
 * ==========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* ------------------------------ 全局区 ------------------------------ */
int        g_init   = 42;              /* 已初始化 → .data */
int        g_uninit;                   /* 未初始化 → .bss  */
int        g_zero   = 0;               /* 显式 0  → 通常也进 .bss */
const int  g_const  = 7;               /* const   → .rodata（MCU 上在 Flash） */
char       g_str[]  = "global";        /* 可写数组 → .data */

/* MCU 上：下面这两个数组会占 RAM；如果写成函数内的非 static 局部数组，就占栈（危险） */
uint8_t    g_buf[256];                 /* .bss */

/* ------------------------------ 函数区 ------------------------------ */
static void inner(void)
{
    int inner_local = 0;               /* 栈（更深一层，地址通常更"低"或更"高"，看栈方向） */
    printf("  inner_local          = %p  (栈，第 2 层)\n", (void*)&inner_local);
}

int main(void)
{
    /* 栈 */
    int        local     = 1;
    int        local2    = 2;
    static int s_init    = 3;          /* static 局部 → .data（不是栈！） */
    static int s_uninit;               /* static 局部 → .bss  */
    /* 堆 */
    char      *heap      = (char*)malloc(16);
    /* 只读区 */
    const char *lit      = "hello";    /* 指针在栈，字符串本体在 .rodata */
    int       (*fp)(void) = NULL;

    if (heap == NULL) { return 1; }

    printf("=== 地址实测（低地址在上）===\n");
    printf("【.rodata / 常量区】\n");
    printf("  &g_const             = %p\n", (void*)&g_const);
    printf("  lit (字符串本体地址)  = %p   \"%s\"\n", (void*)lit, lit);

    printf("【.data 已初始化全局/静态】\n");
    printf("  &g_init              = %p\n", (void*)&g_init);
    printf("  &g_str               = %p   \"%s\"\n", (void*)&g_str, g_str);
    printf("  &s_init (static局部)  = %p  <-- 注意：和栈没关系\n", (void*)&s_init);

    printf("【.bss 未初始化/零值】\n");
    printf("  &g_uninit            = %p\n", (void*)&g_uninit);
    printf("  &g_zero              = %p\n", (void*)&g_zero);
    printf("  &s_uninit(static局部) = %p\n", (void*)&s_uninit);
    printf("  g_buf                = %p   (占 RAM 256 字节，不占 Flash)\n", (void*)g_buf);

    printf("【堆 malloc】\n");
    printf("  heap                 = %p\n", (void*)heap);

    printf("【栈 局部变量】\n");
    printf("  &local               = %p\n", (void*)&local);
    printf("  &local2              = %p\n", (void*)&local2);
    printf("  &lit (指针自己)       = %p\n", (void*)&lit);
    printf("  &fp  (函数指针自己)    = %p\n", (void*)&fp);

    printf("【其他】\n");
    printf("  main 函数地址         = %p   (代码段 .text)\n", (void*)(uintptr_t)&main);
    printf("  sizeof(void*)        = %zu  (本机指针宽度)\n", sizeof(void*));
    inner();

    printf("\n=== 三个自测问题（答案写进笔记，不要只念一遍）===\n");
    printf("1) 哪些地址明显聚成一簇？把每簇对应到 .data/.bss/.rodata/栈/堆。\n");
    printf("2) 为什么 g_uninit 不初始化反而是 0，而 local 不初始化是垃圾值？\n");
    printf("3) 在 STM32 上，g_buf[256] 和函数内的 uint8_t buf[256] 各占谁的空间？\n");

    free(heap);
    heap = NULL;                       /* 铁律 */
    return 0;
}
