#!/bin/bash

# Sanitizer编译和测试脚本
# 演示如何使用不同的Sanitizer编译和运行程序

echo "=== Sanitizer使用演示脚本 ==="
echo "本脚本展示如何使用不同的Sanitizer编译程序并检测bug"
echo

# 检查编译器是否可用
check_compiler() {
    if ! command -v clang &> /dev/null; then
        echo "错误: 需要安装clang编译器"
        echo "Ubuntu/Debian: sudo apt-get install clang"
        echo "CentOS/RHEL: sudo yum install clang"
        exit 1
    fi
}

# 编译函数
compile_with_sanitizer() {
    local sanitizer=$1
    local flags=$2
    local output_name=$3
    
    echo "编译with $sanitizer..."
    clang $flags -o $output_name bug_demonstration.c
    if [ $? -eq 0 ]; then
        echo "✓ 编译成功: $output_name"
    else
        echo "✗ 编译失败"
        return 1
    fi
}

# 运行测试函数
run_test() {
    local executable=$1
    local bug_type=$2
    local sanitizer_name=$3
    
    echo
    echo "--- 使用 $sanitizer_name 运行Bug类型 $bug_type ---"
    ./$executable $bug_type 2>&1 | head -20
    echo
}

check_compiler

cd /home/runner/work/sanitizers/sanitizers/examples

echo "1. 编译不同版本的测试程序..."
echo

# 1. AddressSanitizer版本
echo "编译AddressSanitizer版本..."
compile_with_sanitizer "AddressSanitizer" "-fsanitize=address -g -O1" "demo_asan"

# 2. MemorySanitizer版本 
echo "编译MemorySanitizer版本..."
compile_with_sanitizer "MemorySanitizer" "-fsanitize=memory -g -O1" "demo_msan"

# 3. UndefinedBehaviorSanitizer版本
echo "编译UBSanitizer版本..."
compile_with_sanitizer "UBSanitizer" "-fsanitize=undefined -g -O1" "demo_ubsan"

# 4. 普通版本（无Sanitizer）
echo "编译普通版本（无Sanitizer）..."
compile_with_sanitizer "Normal" "-g -O1" "demo_normal"

echo
echo "2. 运行测试用例..."

# 测试堆缓冲区溢出
echo "=== 测试1: 堆缓冲区溢出 ==="
run_test "demo_normal" "1" "Normal"
run_test "demo_asan" "1" "AddressSanitizer"

# 测试Use-after-free
echo "=== 测试2: Use-after-free ==="
run_test "demo_normal" "2" "Normal" 
run_test "demo_asan" "2" "AddressSanitizer"

# 测试双重释放
echo "=== 测试3: 双重释放 ==="
run_test "demo_normal" "3" "Normal"
run_test "demo_asan" "3" "AddressSanitizer"

# 测试未初始化内存（需要MemorySanitizer）
echo "=== 测试4: 未初始化内存 ==="
run_test "demo_normal" "4" "Normal"
run_test "demo_msan" "4" "MemorySanitizer"

# 测试整数溢出（需要UBSan）
echo "=== 测试6: 整数溢出 ==="
run_test "demo_normal" "6" "Normal"
run_test "demo_ubsan" "6" "UBSanitizer"

echo "=== 测试完成 ==="
echo "从上面的输出可以看到："
echo "1. 普通版本可能不会检测到这些bug"
echo "2. AddressSanitizer能够检测内存访问错误"
echo "3. MemorySanitizer能够检测未初始化内存使用"
echo "4. UBSanitizer能够检测未定义行为"
echo
echo "这些检测能力对于模糊测试工具（如FISHFUZZ和Parmesan）非常重要，"
echo "因为它们能帮助发现传统测试方法难以发现的深层次bug。"