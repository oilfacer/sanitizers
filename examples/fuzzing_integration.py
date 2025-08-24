#!/usr/bin/env python3
"""
模糊测试集成示例
演示如何将Sanitizers集成到模糊测试工作流中
类似于FISHFUZZ和Parmesan的方法
"""

import subprocess
import os
import sys
import time
import random
import string

class SanitizerFuzzer:
    """简化的模糊测试器，展示Sanitizer集成"""
    
    def __init__(self, target_binary):
        self.target_binary = target_binary
        self.crashes_found = 0
        self.total_runs = 0
        self.sanitizer_detections = []
        
    def generate_random_input(self, length=100):
        """生成随机测试输入"""
        return ''.join(random.choices(string.ascii_letters + string.digits, k=length))
    
    def run_target_with_input(self, test_input, bug_type):
        """运行目标程序并检测Sanitizer输出"""
        try:
            # 运行目标程序
            process = subprocess.run(
                [self.target_binary, str(bug_type)],
                input=test_input,
                capture_output=True,
                text=True,
                timeout=5
            )
            
            self.total_runs += 1
            
            # 检查是否有Sanitizer检测到错误
            stderr_output = process.stderr.lower()
            if any(keyword in stderr_output for keyword in 
                   ['sanitizer', 'asan', 'msan', 'ubsan', 'error:', 'crash']):
                self.crashes_found += 1
                detection = {
                    'input': test_input,
                    'bug_type': bug_type,
                    'stderr': process.stderr,
                    'returncode': process.returncode
                }
                self.sanitizer_detections.append(detection)
                return True
                
        except subprocess.TimeoutExpired:
            # 超时也可能表示找到了无限循环等bug
            self.crashes_found += 1
            return True
        except Exception as e:
            print(f"运行错误: {e}")
            
        return False
    
    def run_fuzzing_campaign(self, iterations=100):
        """运行模糊测试活动"""
        print(f"开始模糊测试，目标: {self.target_binary}")
        print(f"计划执行 {iterations} 次测试")
        print("-" * 50)
        
        start_time = time.time()
        
        for i in range(iterations):
            # 随机选择bug类型 (1-7)
            bug_type = random.randint(1, 7)
            
            # 生成随机输入
            test_input = self.generate_random_input()
            
            # 运行测试
            if self.run_target_with_input(test_input, bug_type):
                print(f"✓ 第{i+1}次测试: 发现潜在bug (类型 {bug_type})")
            elif i % 20 == 0:
                print(f"  第{i+1}次测试: 正常执行")
        
        end_time = time.time()
        
        # 输出统计信息
        print("-" * 50)
        print(f"模糊测试完成！")
        print(f"总运行次数: {self.total_runs}")
        print(f"发现bug次数: {self.crashes_found}")
        print(f"bug发现率: {self.crashes_found/self.total_runs*100:.2f}%")
        print(f"运行时间: {end_time-start_time:.2f}秒")
        
        return self.sanitizer_detections

def demonstrate_fishfuzz_approach():
    """演示FISHFUZZ方法：使用多个Sanitizer同时检测"""
    print("=== FISHFUZZ方法演示 ===")
    print("使用多个Sanitizer同时检测不同类型的bug")
    print()
    
    # 不同的Sanitizer版本
    sanitizer_binaries = {
        'AddressSanitizer': './demo_asan',
        'MemorySanitizer': './demo_msan', 
        'UBSanitizer': './demo_ubsan',
        'Normal': './demo_normal'
    }
    
    all_detections = {}
    
    for sanitizer_name, binary in sanitizer_binaries.items():
        if os.path.exists(binary):
            print(f"使用 {sanitizer_name} 进行模糊测试...")
            fuzzer = SanitizerFuzzer(binary)
            detections = fuzzer.run_fuzzing_campaign(50)  # 每个Sanitizer运行50次
            all_detections[sanitizer_name] = detections
            print(f"{sanitizer_name}: 发现 {len(detections)} 个bug")
            print()
    
    return all_detections

def demonstrate_parmesan_approach():
    """演示Parmesan方法：基于梯度的模糊测试"""
    print("=== Parmesan方法演示 ===")
    print("模拟基于梯度的模糊测试（简化版本）")
    print()
    
    # 这里我们模拟Parmesan的梯度引导方法
    # 实际的Parmesan使用更复杂的梯度计算
    
    binary = './demo_asan'
    if not os.path.exists(binary):
        print(f"错误: {binary} 不存在，请先运行编译脚本")
        return
    
    fuzzer = SanitizerFuzzer(binary)
    
    print("阶段1: 随机探索")
    initial_detections = fuzzer.run_fuzzing_campaign(30)
    
    print("\n阶段2: 梯度引导优化")
    print("基于初始发现的bug类型，集中测试相关区域...")
    
    # 模拟梯度引导：如果发现某些bug类型更容易触发，就多测试那些类型
    if initial_detections:
        # 找出最常见的bug类型
        bug_types = [d['bug_type'] for d in initial_detections]
        if bug_types:
            most_common_bug = max(set(bug_types), key=bug_types.count)
            print(f"集中测试bug类型 {most_common_bug}...")
            
            # 集中测试这个bug类型
            for i in range(20):
                test_input = fuzzer.generate_random_input()
                if fuzzer.run_target_with_input(test_input, most_common_bug):
                    print(f"✓ 梯度引导测试 #{i+1}: 发现bug")
    
    print(f"\nParmesan方法总计发现 {len(fuzzer.sanitizer_detections)} 个bug")

def main():
    print("模糊测试与Sanitizer集成演示")
    print("展示FISHFUZZ和Parmesan论文中的方法")
    print("=" * 60)
    print()
    
    # 切换到examples目录
    os.chdir('/home/runner/work/sanitizers/sanitizers/examples')
    
    # 检查是否存在编译好的二进制文件
    required_binaries = ['./demo_asan', './demo_msan', './demo_ubsan', './demo_normal']
    missing_binaries = [b for b in required_binaries if not os.path.exists(b)]
    
    if missing_binaries:
        print("警告: 以下二进制文件不存在:")
        for binary in missing_binaries:
            print(f"  {binary}")
        print("\n请先运行编译脚本: ./run_sanitizer_demo.sh")
        print("或手动编译:")
        print("  clang -fsanitize=address -g -O1 -o demo_asan bug_demonstration.c")
        print("  clang -fsanitize=memory -g -O1 -o demo_msan bug_demonstration.c") 
        print("  clang -fsanitize=undefined -g -O1 -o demo_ubsan bug_demonstration.c")
        print("  clang -g -O1 -o demo_normal bug_demonstration.c")
        return
    
    try:
        # 演示FISHFUZZ方法
        fishfuzz_results = demonstrate_fishfuzz_approach()
        
        print()
        
        # 演示Parmesan方法
        demonstrate_parmesan_approach()
        
        print("\n" + "=" * 60)
        print("总结:")
        print("1. FISHFUZZ通过使用多个Sanitizer扩大了bug检测的覆盖面")
        print("2. Parmesan通过梯度引导提高了模糊测试的效率")
        print("3. 两种方法都大大提高了深层bug的发现能力")
        print("4. Sanitizer是现代模糊测试工具的重要组成部分")
        
    except KeyboardInterrupt:
        print("\n\n用户中断了测试")
    except Exception as e:
        print(f"\n错误: {e}")

if __name__ == "__main__":
    main()