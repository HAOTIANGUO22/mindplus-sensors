# 电位器库验证记录

版本：1.0.0。日期：2026-10-23。

## 输入与硬件约定

用户确认照片中的电位器模块为一个 3P S/V/G 接口。以 S 为模拟输出，Uno 接 A0、掌控板接 P0 实现；供电支持范围和真实端点尚未实测。

## 已通过的软件检查

- Uno 示例：Arduino CLI 1.5.1、arduino:avr:uno 核心 1.8.8，编译链接成功；Flash 4022 字节、RAM 242 字节。
- 掌控板示例：使用本机 Mind+ 的 dfrobot:mpython:mpython 核心 0.0.1，编译链接成功；Flash 210112 字节、RAM 15420 字节。
- 主机 C++ 逻辑测试分别运行 Uno、ESP32 分支，覆盖全部允许引脚、无效引脚、ADC 量程、首样本丢弃、平均采样、百分比、负数/小数/反向输出映射、端点钳制、非法校准保留原配置、反转不影响原始值、重新初始化、无效初始化和 ESP32 ADC 配置。
- TypeScript 生成器测试：全部 10 个积木，两个主控的默认引脚，以及每个引脚、采样次数、方向选项；include/全局对象去重。
- 全部积木生成的草图编译链接成功：Uno Flash 1982 字节、RAM 17 字节；Mind+ 掌控板核心 Flash 204444 字节、RAM 15276 字节。
- 无文字封面已视觉检查，尺寸 600×372；分享 ZIP 检查 CRC、目录配置和图片一致性，不包含 .verification 缓存。

## 复现

以下为本地验证记录。`examples/Uno/`、`examples/Mpython/` 中的 Arduino 草图仅在开发者本地保留，不随 GitHub 或 ZIP 发布；公开示例仅包含 Mind+ `.mp` 项目。下载仓库后可使用生成器测试生成草图进行编译。

从本传感器目录执行命令，使用固定 Arduino CLI 路径：

```powershell
& 'D:\Arduinocli\arduino-cli.exe' compile --fqbn arduino:avr:uno --library arduinoC/libraries/MindPlusPotentiometer examples/Uno
```

验证本机 Mind+ 掌控板核心时，临时 CLI 配置中的 directories.user 指向 `D:/APP/Mind+/Arduino`，并使用 `--build-property 'tools.ctags.path=D:/APP/Mind+/Arduino/arduino-builder'`。这仅适配 CLI 构建旧版 Mind+ 核心，不改动 Mind+ 安装文件。

主机测试以 `tests/Arduino.h` 模拟 ADC：用 C++11 编译器编译 tests/potentiometer_test.cpp 与驱动 CPP，包含 tests 和驱动目录；分别不加宏和加入 `-DESP32` 运行。

运行 `node tests/generator_test.cjs <typescript.js绝对路径>` 产生全部积木的两份草图供编译。工作区统一打包命令为 `python tools/package.py potentiometer`。

## 尚未完成

没有连接真实模块；没有完成 Mind+ 界面内导入、拖拽、上传验收。编译与模拟测试不代表实际模拟电压、噪声、线性度和电位器行程已验证。初始化成功只表示配置的引脚有效。

## Mind+ 示例项目

保留用户保存的 `examples/MindPlus/电位器示例程序.mp`，实际为 A3 原始值程序，SHA256 为 `0DDA69AB03F8A05B1905697AAFDE7A1069E25798BE8AD5495B01B66A2CC7E602`。

另由 `tools/build_mindplus_example.py` 生成 A0 百分比项目，设置串口 115200 和 0.1 秒循环间隔。使用本机 Mind+ 原生保存结构，并从本机安装代码核对串口积木 opcode 与 BAUD 字段。已检查 ZIP CRC、7 个积木节点的父子连接、内嵌扩展版本与驱动一致性、角色资源完整性。此检查不等于在 Mind+ 界面中成功打开、上传或实测。
