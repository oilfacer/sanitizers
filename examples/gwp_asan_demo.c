/*
 * GWP-ASan演示程序
 * 展示采样式内存错误检测，这是FISHFUZZ和Parmesan等工具的核心技术
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// 模拟GWP-ASan的采样机制
static int sample_rate = 128;  // 1/128的采样率
static int allocation_counter = 0;

// 简化的采样决策函数
int should_sample() {
    allocation_counter++;
    return (allocation_counter % sample_rate) == 0;
}

// 模拟大量内存分配，只有部分会被"监控"
void simulate_gwp_asan_sampling() {
    printf("=== GWP-ASan采样演示 ===\n");
    printf("采样率: 1/%d\n", sample_rate);
    printf("进行1000次内存分配，观察采样情况...\n\n");
    
    int sampled_count = 0;
    int total_allocations = 1000;
    
    for (int i = 0; i < total_allocations; i++) {
        void *ptr = malloc(64);
        
        if (should_sample()) {
            sampled_count++;
            printf("分配 #%d: 被采样监控 (指针: %p)\n", i, ptr);
        }
        
        free(ptr);
    }
    
    printf("\n总分配次数: %d\n", total_allocations);
    printf("被采样监控的分配: %d\n", sampled_count);
    printf("实际采样率: %.2f%%\n", (double)sampled_count / total_allocations * 100);
}

// 模拟在模糊测试中可能触发的复杂bug
void complex_bug_scenario() {
    printf("\n=== 复杂Bug场景演示 ===\n");
    printf("模拟模糊测试中可能发现的复杂内存错误...\n");
    
    // 分配多个缓冲区
    char *buffers[10];
    for (int i = 0; i < 10; i++) {
        buffers[i] = malloc(100);
        snprintf(buffers[i], 100, "Buffer %d content", i);
    }
    
    // 释放部分缓冲区
    for (int i = 0; i < 5; i++) {
        free(buffers[i]);
        buffers[i] = NULL;
    }
    
    // 模拟模糊测试输入导致的错误访问
    // 这种错误在没有Sanitizer的情况下很难被发现
    printf("尝试访问已释放的内存...\n");
    
    // 这里故意触发use-after-free错误
    // 在实际的模糊测试中，这种错误可能只在特定输入下才会触发
    if (buffers[2] != NULL) {  // 这个检查在释放后是无效的
        strcpy(buffers[2], "触发use-after-free错误");
    }
    
    // 清理剩余的缓冲区
    for (int i = 5; i < 10; i++) {
        free(buffers[i]);
    }
}

// 演示数据流敏感的错误检测
void data_flow_sensitive_bug() {
    printf("\n=== 数据流敏感Bug演示 ===\n");
    printf("这种错误需要跟踪数据流才能发现，正是Parmesan论文中讨论的情况\n");
    
    char *source = malloc(50);
    char *dest = malloc(50);
    
    // 初始化源数据
    strcpy(source, "sensitive_data");
    
    // 复杂的数据流转换
    memcpy(dest, source, strlen(source));
    
    // 释放源内存
    free(source);
    
    // 通过dest间接访问已释放的内存区域
    // 这种错误需要跟踪数据流依赖关系才能发现
    printf("通过间接引用访问数据: %s\n", dest);
    
    // 这里可能触发错误，取决于内存布局
    dest[strlen(dest)] = '!';  // 可能越界
    
    free(dest);
}

// 演示时间相关的bug
void timing_sensitive_bug() {
    printf("\n=== 时间敏感Bug演示 ===\n");
    printf("这种错误只在特定时间条件下触发，模糊测试很难发现\n");
    
    char *ptr1 = malloc(100);
    char *ptr2 = malloc(100);
    
    // 模拟时间延迟
    usleep(1000);  // 1毫秒延迟
    
    free(ptr1);
    
    // 在某些情况下，新的分配可能重用刚释放的内存
    char *ptr3 = malloc(100);
    
    // 如果ptr3重用了ptr1的地址，这可能导致混淆
    if (ptr3 == ptr1) {
        printf("内存地址被重用！这可能导致错误\n");
    }
    
    // 潜在的错误：如果代码错误地认为ptr1仍然有效
    // strcpy(ptr1, "错误使用已释放的内存");
    
    free(ptr2);
    free(ptr3);
}

int main(int argc, char *argv[]) {
    printf("GWP-ASan和模糊测试演示程序\n");
    printf("展示FISHFUZZ和Parmesan论文中提到的Sanitizer技术\n");
    printf("================================\n\n");
    
    // 初始化随机数种子
    srand(time(NULL));
    
    // 演示GWP-ASan的采样机制
    simulate_gwp_asan_sampling();
    
    // 演示复杂的bug场景
    complex_bug_scenario();
    
    // 演示数据流敏感的错误
    data_flow_sensitive_bug();
    
    // 演示时间敏感的错误
    timing_sensitive_bug();
    
    printf("\n=== 总结 ===\n");
    printf("1. GWP-ASan通过采样减少性能开销，适合生产环境\n");
    printf("2. 模糊测试结合Sanitizer能发现复杂的内存错误\n");
    printf("3. FISHFUZZ使用多种Sanitizer提高bug检测覆盖率\n");
    printf("4. Parmesan利用Sanitizer反馈进行梯度引导的模糊测试\n");
    printf("5. 这些技术的结合大大提高了漏洞发现的效率\n");
    
    return 0;
}