# Makefile for Sanitizer demonstrations
# 展示如何使用不同的Sanitizer编译程序

CC = clang
CFLAGS = -g -O1 -Wall

# 源文件
SOURCES = bug_demonstration.c gwp_asan_demo.c

# 目标文件
TARGETS = demo_normal demo_asan demo_msan demo_ubsan demo_tsan gwp_demo

.PHONY: all clean test help

all: $(TARGETS)
	@echo "所有Sanitizer演示程序编译完成！"
	@echo "运行 'make test' 来执行演示"

# 普通版本（无Sanitizer）
demo_normal: bug_demonstration.c
	$(CC) $(CFLAGS) -o $@ $<

# AddressSanitizer版本
demo_asan: bug_demonstration.c
	$(CC) $(CFLAGS) -fsanitize=address -o $@ $<

# MemorySanitizer版本
demo_msan: bug_demonstration.c
	$(CC) $(CFLAGS) -fsanitize=memory -o $@ $<

# UndefinedBehaviorSanitizer版本
demo_ubsan: bug_demonstration.c
	$(CC) $(CFLAGS) -fsanitize=undefined -o $@ $<

# ThreadSanitizer版本
demo_tsan: bug_demonstration.c
	$(CC) $(CFLAGS) -fsanitize=thread -o $@ $<

# GWP-ASan演示
gwp_demo: gwp_asan_demo.c
	$(CC) $(CFLAGS) -o $@ $<

# 测试所有版本
test: $(TARGETS)
	@echo "=== 运行Sanitizer演示测试 ==="
	@echo "1. 运行基本的bug演示脚本..."
	./run_sanitizer_demo.sh
	@echo ""
	@echo "2. 运行GWP-ASan演示..."
	./gwp_demo
	@echo ""
	@echo "3. 运行模糊测试集成演示..."
	python3 fuzzing_integration.py

# 运行特定的bug类型测试
test-bug-%: demo_asan demo_normal
	@echo "=== 测试Bug类型 $* ==="
	@echo "--- 无Sanitizer版本 ---"
	-./demo_normal $* 2>&1 || true
	@echo ""
	@echo "--- AddressSanitizer版本 ---"
	-./demo_asan $* 2>&1 || true
	@echo ""

# 性能比较测试
benchmark: $(TARGETS)
	@echo "=== 性能比较测试 ==="
	@echo "测试用例：堆缓冲区溢出检测"
	@echo ""
	@echo "普通版本："
	time -p ./demo_normal 1 2>/dev/null || true
	@echo ""
	@echo "AddressSanitizer版本："
	time -p ./demo_asan 1 2>/dev/null || true
	@echo ""

# 清理编译产物
clean:
	rm -f $(TARGETS)
	rm -f *.o
	@echo "清理完成"

# 帮助信息
help:
	@echo "Sanitizer演示程序 Makefile"
	@echo ""
	@echo "可用目标："
	@echo "  all           - 编译所有演示程序"
	@echo "  test          - 运行完整的演示测试"
	@echo "  test-bug-N    - 测试特定的bug类型 (N=1-7)"
	@echo "  benchmark     - 运行性能比较测试"
	@echo "  clean         - 清理编译产物"
	@echo "  help          - 显示此帮助信息"
	@echo ""
	@echo "示例用法："
	@echo "  make all              # 编译所有程序"
	@echo "  make test             # 运行完整测试"
	@echo "  make test-bug-1       # 只测试堆缓冲区溢出"
	@echo "  make benchmark        # 性能比较"
	@echo ""
	@echo "单独运行程序："
	@echo "  ./demo_asan 1         # 用ASan测试堆缓冲区溢出"
	@echo "  ./demo_msan 4         # 用MSan测试未初始化内存"
	@echo "  ./gwp_demo            # 运行GWP-ASan演示"