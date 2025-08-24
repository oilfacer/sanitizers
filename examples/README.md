# Sanitizer实践示例

本目录包含了完整的Sanitizer使用演示，展示了FISHFUZZ和Parmesan等研究论文中提到的技术。

## 文件说明

- `bug_demonstration.c` - 演示各种内存安全bug的示例程序
- `gwp_asan_demo.c` - GWP-ASan采样机制演示程序  
- `fuzzing_integration.py` - 模糊测试集成演示脚本
- `run_sanitizer_demo.sh` - 自动编译和运行所有演示的脚本
- `Makefile` - 便于编译和测试的Make文件

## 快速开始

### 方法1：使用Makefile（推荐）

```bash
# 编译所有演示程序
make all

# 运行完整的演示测试
make test

# 只测试特定的bug类型（1-7）
make test-bug-1  # 测试堆缓冲区溢出

# 运行性能比较测试
make benchmark

# 查看帮助
make help
```

### 方法2：使用自动化脚本

```bash
# 运行基本的Sanitizer演示
./run_sanitizer_demo.sh

# 运行GWP-ASan演示
make gwp_demo && ./gwp_demo

# 运行模糊测试集成演示
python3 fuzzing_integration.py
```

### 方法3：手动编译

```bash
# AddressSanitizer版本
clang -fsanitize=address -g -O1 -o demo_asan bug_demonstration.c

# MemorySanitizer版本
clang -fsanitize=memory -g -O1 -o demo_msan bug_demonstration.c

# UndefinedBehaviorSanitizer版本
clang -fsanitize=undefined -g -O1 -o demo_ubsan bug_demonstration.c

# 运行测试
./demo_asan 1  # 测试堆缓冲区溢出
./demo_msan 4  # 测试未初始化内存
```

## 支持的Bug类型

1. **堆缓冲区溢出** - AddressSanitizer检测
2. **Use-after-free** - AddressSanitizer检测  
3. **双重释放** - AddressSanitizer检测
4. **未初始化内存使用** - MemorySanitizer检测
5. **栈缓冲区溢出** - AddressSanitizer检测
6. **整数溢出** - UndefinedBehaviorSanitizer检测
7. **空指针解引用** - AddressSanitizer检测

## 与研究论文的对应关系

### FISHFUZZ: "Catch Deeper Bugs by Throwing Larger Nets"

- **多重Sanitizer使用**：通过同时使用ASan、MSan、UBSan等扩大检测覆盖面
- **采样策略**：GWP-ASan演示了如何在生产环境中低开销地检测错误
- **深层bug发现**：复杂场景演示了传统测试难以发现的错误

### Parmesan研究

- **梯度引导**：`fuzzing_integration.py`展示了基于Sanitizer反馈的智能模糊测试
- **敏感性分析**：数据流敏感bug演示了Parmesan论文中的关键概念
- **效率优化**：采样机制展示了如何平衡检测精度和性能开销

## 性能开销对比

| Sanitizer | 内存开销 | 运行时开销 | 适用场景 |
|-----------|----------|------------|----------|
| Normal    | 1x       | 1x         | 基准测试 |
| ASan      | 2-3x     | 2x         | 开发测试 |
| MSan      | 3x       | 3x         | 开发测试 |
| UBSan     | 最小     | 10-20%     | 任何环境 |
| GWP-ASan  | ~0%      | ~0%        | 生产环境 |

## 实际应用建议

1. **开发阶段**：使用ASan + UBSan进行全面检测
2. **测试阶段**：添加MSan检测逻辑错误
3. **生产环境**：部署GWP-ASan进行持续监控
4. **模糊测试**：结合多种Sanitizer提高bug发现率

## 故障排除

如果遇到编译错误：
- 确保安装了clang编译器：`sudo apt-get install clang`
- 检查是否支持所需的Sanitizer：`clang -fsanitize=address -x c -E /dev/null`

如果某些bug没有被检测到：
- 尝试调整编译优化级别：`-O0`而不是`-O1`
- 设置Sanitizer运行时选项：`export ASAN_OPTIONS="abort_on_error=1"`

## 进一步学习

- 查看主目录的`SANITIZER_USAGE_GUIDE.md`获取详细指南
- 研究`android/app/`目录中的Android应用示例
- 阅读`gwp-asan/icse2024/`目录中的研究论文