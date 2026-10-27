/* ============================================================================
 *  pointer_drills.c  ——  第 1 周 C 语言指针练习（T1 ~ T10）
 * ----------------------------------------------------------------------------
 *  怎么用：
 *    1. 每道题的"题意"写在函数上方的注释里，函数体里是**故意写错的桩代码**，
 *       第一遍编译运行会看到一排 FAIL，这是正常的。
 *    2. 按 T1 → T10 顺序做，做完一道就重跑一次，看到 PASS 再下一道。
 *    3. 每道题做完，在这道题注释的最后一行写一句"考点是什么"（这是笔记的一部分）。
 *    4. 全部 PASS 后，把本文件提交到 embedded-notes 仓库。
 *
 *  在哪跑（本机目前没有主机 gcc，三种任选）：
 *    A) 推荐：装个 Windows 版 gcc —— w64devkit（免安装 zip，解压即用）
 *       或 MSYS2；然后  gcc -std=c11 -Wall -Wextra -o drills pointer_drills.c && ./drills
 *       （第 8 周要学 Makefile/GCC，现在装好是提前投资）
 *    B) 在线编译器（浏览器可用）：搜 "onlinegdb" 或 "菜鸟工具 C 在线编译器"，
 *       把本文件粘进去点运行。
 *    C) 只用 Keil 做**语法检查**：新建工程把本文件加进去，编译过 0 error 即可，
 *       但运行要看结果，还是得 A 或 B。
 *
 *  纪律：不许用下标 p[i]（T7/T10 除外），全部用 *p / *(p+i) 写；
 *        T8/T9 不许调用 strlen/memset，自己用指针遍历实现。
 *
 *  编码：本文件是 UTF-8 with BOM。
 *        实测结论：Keil 的 armcc 在中文 Windows 下默认按 GBK 读源码，
 *        如果中文注释后面紧跟英文引号（例如 "指针）"），收尾引号会被当成 GBK 尾字节吃掉，
 *        报错 `#8: missing closing quote` 或 `improperly terminated macro invocation`。
 *        加 BOM 后 armcc 就按 UTF-8 解析，正常通过。
 *        → 以后你自己写含中文的 .c 文件，一律存成 "UTF-8 with BOM"（VSCode 右下角可切）。
 * ==========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

/* ------------------------------------------------------------------ 测试框架 */
static int g_pass = 0, g_fail = 0;

#define CHECK(cond, name)                                                  \
    do {                                                                   \
        if (cond) { g_pass++; printf("  [PASS] %s\n", name); }              \
        else      { g_fail++; printf("  [FAIL] %s   <-- 再想想\n", name); } \
    } while (0)

/* ================================================================== T1
 * 题意：返回 (*p) + 1 的值。
 *       用来体会：解引用、以及 `*p + 1` 与 `*(p + 1)` 的区别。
 * 考点（做完自己写）：
 */
static int t1_deref(const int *p)
{
    (void)p;
    return -999;                 /* TODO */
}

/* ================================================================== T2
 * 题意：交换 *x 和 *y 指向的两个 int。函数体内不许出现 x = 或 y =（只能改 *x / *y）。
 * 考点：
 */
static void t2_swap(int *x, int *y)
{
    (void)x; (void)y;
    /* TODO */
}

/* ================================================================== T3
 * 题意：把数组 a 的 n 个元素每个加 1。只能用指针遍历（`int *p = a; p < a + n; ++p`）。
 * 考点：
 */
static void t3_inc_all(int *a, size_t n)
{
    (void)a; (void)n;
    /* TODO */
}

/* ================================================================== T4
 * 题意：原地反转数组 a 的 n 个元素（只用指针与交换，处理 n == 0 / n == 1）。
 * 考点：
 */
static void t4_reverse(int *a, size_t n)
{
    (void)a; (void)n;
    /* TODO */
}

/* ================================================================== T5
 * 题意：返回指针 b 与 a 之间的"元素个数"（b 在 a 之后时为正）。
 *       用来体会：指针相减得到的是元素个数，不是字节数。
 * 考点：
 */
static ptrdiff_t t5_steps(const int *a, const int *b)
{
    (void)a; (void)b;
    return -999;                 /* TODO */
}

/* ================================================================== T6
 * 题意：让 *pp 指向一块新分配的 int，把它的值设为 42，返回 0；分配失败返回 -1。
 *       用来体会：要修改"指针本身"，必须传二级指针。
 * 考点：
 */
static int t6_change_ptr(int **pp)
{
    (void)pp;
    return -1;                   /* TODO */
}

/* ================================================================== T7
 * 题意：在字符串数组 names（n 个元素）里找与 key 完全相同的字符串，
 *       返回它的地址；找不到返回 NULL。可用 strcmp。
 *       用来体会：char ** 遍历与"返回指针"。
 * 考点：
 */
static const char *t7_pick(const char **names, size_t n, const char *key)
{
    (void)names; (void)n; (void)key;
    return NULL;                 /* TODO */
}

/* ================================================================== T8
 * 题意：自己实现 strlen：返回 s 的字符个数（不含 '\0'）。不许调用 strlen。
 *       用来体会：字符串就是"以 '\0' 结尾的字符数组"，指针走到 0 就停。
 * 考点：
 */
static size_t t8_count(const char *s)
{
    (void)s;
    return 0;                    /* TODO */
}

/* ================================================================== T9
 * 题意：把 buf 的前 n 个字节都写成 v。不许调用 memset。
 *       用来体会：mem* 系列函数的本质就是字节指针遍历。
 * 考点：
 */
static void t9_fill(uint8_t *buf, size_t n, uint8_t v)
{
    (void)buf; (void)n; (void)v;
    /* TODO */
}

/* ================================================================== T10
 * 题意：从 buf[0]、buf[1] 解析出一个**小端**的 uint16_t 并返回。
 *       不许写成 *(uint16_t*)buf（想一想为什么）。
 *       用来体会：协议解析靠逐字节拼接，不依赖对齐、大小端显式。
 * 考点：
 */
static uint16_t t10_le16(const uint8_t *buf)
{
    (void)buf;
    return 0;                    /* TODO */
}

/* ==================================================================== main */
int main(void)
{
    printf("=== 第1周 指针练习 T1~T10 ===\n");

    /* T1 */
    printf("T1\n");
    {
        int a  = 10;
        int arr[3] = {10, 20, 30};
        CHECK(t1_deref(&a) == 11, "t1_deref 返回 *p + 1 == 11");
        CHECK(t1_deref(arr) == 11, "t1_deref 传入数组名同样得 11（数组名退化为指针）");
        /* 想一想：t1_deref(arr + 1) 应该是多少？把答案写在下面这行注释里，再改上面的 CHECK 验证 */
        /* 答案： */
    }

    /* T2 */
    printf("T2\n");
    {
        int x = 1, y = 2;
        t2_swap(&x, &y);
        CHECK(x == 2 && y == 1, "t2_swap 交换成功");
    }

    /* T3 */
    printf("T3\n");
    {
        int a[4] = {1, 2, 3, 4};
        t3_inc_all(a, 4);
        CHECK(a[0] == 2 && a[1] == 3 && a[2] == 4 && a[3] == 5, "t3_inc_all 每个元素 +1");
        t3_inc_all(a, 0);                 /* 边界：n = 0 不能崩 */
        CHECK(a[0] == 2, "t3_inc_all n=0 安全");
    }

    /* T4 */
    printf("T4\n");
    {
        int a[5] = {1, 2, 3, 4, 5};
        int b[1] = {7};
        t4_reverse(a, 5);
        CHECK(a[0] == 5 && a[1] == 4 && a[2] == 3 && a[3] == 2 && a[4] == 1, "t4_reverse 奇数个元素");
        t4_reverse(a, 0);
        t4_reverse(b, 1);
        CHECK(b[0] == 7, "t4_reverse n=0/1 安全");
    }

    /* T5 */
    printf("T5\n");
    {
        int a[5] = {0};
        CHECK(t5_steps(&a[0], &a[3]) == 3, "t5_steps == 3（元素个数，不是 12 字节）");
        CHECK(t5_steps(&a[3], &a[0]) == -3, "t5_steps 反向为负");
    }

    /* T6 */
    printf("T6\n");
    {
        int *p = NULL;
        int rc = t6_change_ptr(&p);
        CHECK(rc == 0 && p != NULL, "t6_change_ptr 返回 0 且 p 非空");
        CHECK(p != NULL && *p == 42, "t6_change_ptr 写入了 42");
        free(p);                        /* 谁分配谁释放 */
        p = NULL;                       /* 铁律：free 后置空 */
    }

    /* T7 */
    printf("T7\n");
    {
        const char *names[] = {"led", "uart", "i2c", "spi"};
        const char *hit = t7_pick(names, 4, "i2c");
        CHECK(hit != NULL && strcmp(hit, "i2c") == 0, "t7_pick 找到 i2c");
        CHECK(t7_pick(names, 4, "can") == NULL, "t7_pick 找不到返回 NULL");
        /* 小问：hit 应该等于 names[2]，也就是同一个地址，为什么？做完在这里写一行注释 */
    }

    /* T8 */
    printf("T8\n");
    {
        CHECK(t8_count("") == 0, "t8_count 空串 == 0");
        CHECK(t8_count("stm32") == 5, "t8_count \"stm32\" == 5");
        CHECK(t8_count("a\0b") == 1, "t8_count 遇 '\\0' 停");
    }

    /* T9 */
    printf("T9\n");
    {
        uint8_t buf[8];
        size_t i;
        for (i = 0; i < 8; i++) buf[i] = 0xAA;
        t9_fill(buf, 4, 0x00);
        CHECK(buf[0] == 0 && buf[3] == 0 && buf[4] == 0xAA, "t9_fill 只写前 4 字节");
        t9_fill(buf, 0, 0xFF);
        CHECK(buf[0] == 0, "t9_fill n=0 安全");
    }

    /* T10 */
    printf("T10\n");
    {
        const uint8_t frame[4] = {0x01, 0x00, 0x34, 0x12};
        CHECK(t10_le16(frame) == 0x0001, "t10_le16 低字节在前");
        CHECK(t10_le16(frame + 2) == 0x1234, "t10_le16 解析 0x34,0x12 -> 0x1234");
        /* 想一想：为什么 buf 起始地址是奇数时，*(uint16_t*)buf 会出问题？ */
    }

    printf("\n=== 结果：%d PASS / %d FAIL ===\n", g_pass, g_fail);
    if (g_fail == 0) {
        printf("全部通过。别忘了回每道题的注释里补上「考点」。\n");
    } else {
        printf("还有 %d 道没过，逐道修，别一次全改。\n", g_fail);
    }
    return g_fail == 0 ? 0 : 1;
}
