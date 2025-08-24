/*
 * 实践示例：演示不同类型的内存安全Bug
 * 这些Bug类型经常在FISHFUZZ和Parmesan等模糊测试工具中被发现
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 示例1：堆缓冲区溢出 (Heap Buffer Overflow)
// 这是模糊测试最常发现的bug类型之一
void heap_buffer_overflow_example() {
    printf("=== 堆缓冲区溢出示例 ===\n");
    char *buffer = (char*)malloc(10);
    // 故意写入超出分配范围的内存
    buffer[15] = 'X';  // AddressSanitizer会检测到这个错误
    free(buffer);
}

// 示例2：Use-after-free错误
// 在模糊测试中，复杂的控制流可能导致这类错误
void use_after_free_example() {
    printf("=== Use-after-free示例 ===\n");
    char *ptr = (char*)malloc(100);
    free(ptr);
    // 使用已释放的内存
    ptr[0] = 'A';  // AddressSanitizer会检测到这个错误
}

// 示例3：双重释放 (Double Free)
// 复杂的代码路径可能导致同一内存被释放多次
void double_free_example() {
    printf("=== 双重释放示例 ===\n");
    char *ptr = (char*)malloc(50);
    free(ptr);
    free(ptr);  // AddressSanitizer会检测到这个错误
}

// 示例4：未初始化内存使用
// MemorySanitizer专门检测这类错误
void uninitialized_memory_example() {
    printf("=== 未初始化内存使用示例 ===\n");
    int uninit_var;
    // 使用未初始化的变量
    if (uninit_var > 10) {  // MemorySanitizer会检测到这个错误
        printf("条件为真\n");
    }
}

// 示例5：栈缓冲区溢出
// 经典的安全漏洞类型
void stack_buffer_overflow_example() {
    printf("=== 栈缓冲区溢出示例 ===\n");
    char buffer[10];
    // 写入超出栈缓冲区的数据
    strcpy(buffer, "这是一个很长的字符串，会导致栈溢出");  // AddressSanitizer会检测到
}

// 示例6：整数溢出
// UndefinedBehaviorSanitizer可以检测
void integer_overflow_example() {
    printf("=== 整数溢出示例 ===\n");
    int max_int = 2147483647;  // INT_MAX
    int result = max_int + 1;  // UBSan会检测到有符号整数溢出
    printf("结果: %d\n", result);
}

// 示例7：空指针解引用
// 常见的运行时错误
void null_dereference_example() {
    printf("=== 空指针解引用示例 ===\n");
    char *null_ptr = NULL;
    *null_ptr = 'A';  // AddressSanitizer会检测到这个错误
}

// 主函数：根据参数选择运行哪个示例
int main(int argc, char *argv[]) {
    printf("Sanitizer Bug演示程序\n");
    printf("用法: %s <bug_type>\n", argv[0]);
    printf("Bug类型:\n");
    printf("  1 - 堆缓冲区溢出\n");
    printf("  2 - Use-after-free\n");
    printf("  3 - 双重释放\n");
    printf("  4 - 未初始化内存\n");
    printf("  5 - 栈缓冲区溢出\n");
    printf("  6 - 整数溢出\n");
    printf("  7 - 空指针解引用\n");
    
    if (argc < 2) {
        printf("请指定要演示的bug类型\n");
        return 1;
    }
    
    int bug_type = atoi(argv[1]);
    
    switch (bug_type) {
        case 1:
            heap_buffer_overflow_example();
            break;
        case 2:
            use_after_free_example();
            break;
        case 3:
            double_free_example();
            break;
        case 4:
            uninitialized_memory_example();
            break;
        case 5:
            stack_buffer_overflow_example();
            break;
        case 6:
            integer_overflow_example();
            break;
        case 7:
            null_dereference_example();
            break;
        default:
            printf("未知的bug类型: %d\n", bug_type);
            return 1;
    }
    
    return 0;
}