# 软件验证记录

日期：2026-10-23。版本：1.0.0。

## 已完成

- Arduino CLI 1.5.1，官方 AVR 核心 1.8.8：Uno 示例编译、链接成功。Flash 3676 字节，RAM 263 字节。
- Arduino CLI 1.5.1，`D:/APP/Mind+/Arduino/hardware/dfrobot/mpython` 自带核心 0.0.1：掌控板示例编译、链接成功。Flash 207820 字节，RAM 15324 字节。
- C++ 主机测试：正反转、触点来回抖动、半步回退、非法双位跳变、方向反转、1/2/4 边沿倍率、整数边界饱和、设置计数、变化量消费、20ms 按键消抖、长按、事件消费、millis 回绕、第二个对象拒绝、重复初始化。
- TypeScript 转译与生成器测试：11 个积木函数、两种主控默认引脚、include/对象去重；生成完整 Arduino 草图供编译检查。
- 全部积木生成的草图已分别编译、链接成功：Uno Flash 2194 字节、RAM 40 字节；Mind+ 掌控板核心 Flash 202460 字节、RAM 15172 字节。
- 已核对本机 Mind+ 1.x 的菜单处理代码：`_menus/<board>.json`、`default_<函数>_<参数>` 的读取形式与本库一致。
- ZIP 已执行 CRC 完整性检查，包含导入配置、驱动、积木、菜单、封面、说明及示例。

## 界面验收状态

尝试通过 Windows Computer Use 启动 Mind+ 时返回 `GetCursorPos failed: 拒绝访问。 (0x80070005)`，重新查询窗口列表也没有 Mind+ 窗口。因此没有完成界面内导入、积木拖拽和官方 `.mpext` 导出验证；不将 TypeScript 生成器测试标为 UI 验收。

## 复现

以下为本地验证记录。`examples/Uno/`、`examples/Mpython/` 中的 Arduino 草图仅在开发者本地保留，不随 GitHub 或 ZIP 发布；公开示例仅包含 Mind+ `.mp` 项目。下载仓库后可使用生成器测试生成草图进行编译。

在传感器根目录 `sensors/rotary-encoder/` 运行。使用本机固定 Arduino CLI 路径：

```powershell
& 'D:\Arduinocli\arduino-cli.exe' compile --fqbn arduino:avr:uno --library 'arduinoC/libraries/MindPlusRotaryEncoder' examples/Uno
```

掌控板使用临时 CLI 配置，内容如下，不改动系统 Arduino 配置：

```yaml
directories:
  user: D:/APP/Mind+/Arduino
```

```powershell
& 'D:\Arduinocli\arduino-cli.exe' --config-file '.verification/mindplus-cli.yaml' compile --fqbn dfrobot:mpython:mpython --build-property 'tools.ctags.path=D:/APP/Mind+/Arduino/arduino-builder' --library 'arduinoC/libraries/MindPlusRotaryEncoder' examples/Mpython
```

`tools.ctags.path` 覆盖仅用于 CLI 兼容 Mind+ 自带的旧版构建配置；不需要修改 Mind+ 安装文件，用户在 Mind+ 内使用时也不需要这个命令。

主机逻辑测试需 C++11 编译器，`tests/Arduino.h` 是主机测试替身，未随 Arduino 驱动一起引用；生成器测试需要本地 TypeScript 模块路径：

```text
g++ -std=c++11 -Wall -Wextra -Werror -Itests -IarduinoC/libraries/MindPlusRotaryEncoder tests/encoder_test.cpp arduinoC/libraries/MindPlusRotaryEncoder/MindPlusRotaryEncoder.cpp -o encoder-test.exe
node tests/generator_test.cjs <typescript.js的绝对路径>
```

## 实物边界

尚未连接实物。4P 为 A/B、3P 为低有效按键是依据照片和接口描述作出的假设；供电范围、机械每格脉冲数和顺时针极性需要实物确认。模拟测试不覆盖真实电气噪声、高速漏脉冲或其他库长时间关闭中断的情况。

## 1.0.1 封面更新

替换为根据实物照片生成的卡通硬件封面，尺寸 600×372；内置 image_gen 生成，保留原始图及提示词。驱动和积木未改变，沿用上述编译验证。用户反馈 1.0.0 已成功导入；1.0.1 新封面尚待 Mind+ 界面刷新确认。打包脚本核对尺寸及 ZIP 内封面与本地一致。

## 1.0.2 无文字封面更新

按用户指定的局部图片，通过内置 image_gen 制作无文字横版封面，只保留卡通硬件、背景和旋转箭头。600×372，已视觉检查；驱动与积木未修改。ZIP 校验完整性及封面一致性。

## 多传感器目录整理

源码移至 sensors/rotary-encoder，发布文件移至 releases/rotary-encoder。保留用户 .mp 项目与 .mpext 导出，检查二者内含 config.json 的版本为 1.0.2；未改写文件内容。源码 ZIP 增加用户 .mp 示例，当前版本号仍为 1.0.2，驱动和积木未修改。整理前的 ZIP 保留在发布目录 archive 中。

旧 .verification 编译目录中可能保存整理前的绝对路径，作为历史缓存保留；重新编译时建议选择新的 --build-path，不直接复用旧缓存。
