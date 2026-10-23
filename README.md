# Mind+ 传感器扩展库工作区

每个传感器独立开发，分享文件集中存放。当前已有旋转编码器和电位器，均面向 Mind+ 1.x 上传模式的 Uno 和掌控板；各库的验证范围见各自 VALIDATION.md。

仓库及发布 ZIP 中的示例仅提供 Mind+ `.mp` 积木项目。Arduino `.ino` 验证草图仅在开发者本地保留，不上传。

## 下载和使用

| 传感器 | 完整源码分享包 | Mind+ 积木示例 |
|---|---|---|
| 旋转编码器 | [1.0.2 ZIP](releases/rotary-encoder/MindPlus-RotaryEncoder-1.0.2.zip) | [用户保存的示例](releases/rotary-encoder/编码器示例程序.mp) |
| 电位器 | [1.0.0 ZIP](releases/potentiometer/MindPlus-Potentiometer-1.0.0.zip) | [Uno A0 百分比](releases/potentiometer/电位器_Uno_A0_百分比示例.mp) · [用户 A3 原始值示例](releases/potentiometer/电位器示例程序.mp) |

在 GitHub 文件页点击下载原始文件。源码 ZIP 先解压，再从 Mind+ 用户库导入对应的 config.json；.mp 通过 Mind+“项目 → 打开”打开。编码器另有[已导出的 .mpext](releases/rotary-encoder/haotianguo22-rotaryencoder-thirdex-V1.0.2.mpext)，可直接从用户库导入。

这是多个传感器组成的开发仓库，根目录没有单个扩展的 config.json。请下载对应的包或选择 sensors 下某个库的 config.json，不要把整个仓库地址当作单一 Mind+ 用户库导入。

## 常用入口

| 要做什么 | 去哪里 |
|---|---|
| 修改编码器积木、引脚或驱动 | [编码器源码](sensors/rotary-encoder/) |
| 导入编码器用户库 | [config.json](sensors/rotary-encoder/config.json) |
| 打开搭好的积木示例 | [编码器示例程序.mp](sensors/rotary-encoder/examples/MindPlus/编码器示例程序.mp) |
| 找发给别人的文件 | [编码器发布文件夹](releases/rotary-encoder/) |
| 了解接线和积木搭法 | [编码器使用说明](sensors/rotary-encoder/README.md) |
| 导入电位器用户库 | [电位器 config.json](sensors/potentiometer/config.json) |
| 电位器接线、积木和校准 | [电位器使用说明](sensors/potentiometer/README.md) |
| 分享电位器库 | [电位器发布文件夹](releases/potentiometer/) |
| 开发下一个传感器 | [开发约定](docs/开发约定.md) |

## 目录

```text
mind+扩展库/
├─ README.md                         工作区导航
├─ docs/
│  └─ 开发约定.md                    后续传感器的目录和开发方式
├─ tools/
│  └─ package.py                     统一打包入口
├─ sensors/                          每个传感器一个独立文件夹
│  ├─ potentiometer/                 电位器（同样拥有独立配置、驱动和示例）
│  └─ rotary-encoder/                旋转编码器
│     ├─ config.json                 Mind+ 本地导入入口
│     ├─ arduinoC/                   积木、菜单、封面、Arduino 驱动
│     ├─ assets/                     封面原图、生成提示词
│     ├─ examples/
│     │  └─ MindPlus/                .mp 积木示例的原始文件
│     ├─ tests/                      验证脚本
│     ├─ tools/                      此传感器的打包脚本
│     ├─ .verification/              本地编译缓存与日志，不进入分享包
│     ├─ README.md                   使用说明
│     ├─ VALIDATION.md               验证记录
│     ├─ 分享说明.txt
│     └─ LICENSE.TXT
└─ releases/
   ├─ potentiometer/                 电位器源码分享包和说明
   └─ rotary-encoder/
      ├─ *.mpext                     从 Mind+ 导出的用户库
      ├─ 编码器示例程序.mp           发布用示例副本
      ├─ MindPlus-RotaryEncoder-*.zip 完整源码与示例分享包
      ├─ 分享说明.txt
      └─ archive/                    整理前的旧分享包，保留备查
```

## 路径调整后的首次使用

编码器原来放在工作区根目录，现已移到 `sensors/rotary-encoder/`。Mind+ 如果还记着旧的本地路径，请从新位置重新导入 `config.json`；已安装的 .mpext 是否需要重新导入以软件实际状态为准。

你提供的 `.mp` 和 `.mpext` 已原样保留，并核对文件哈希。新路径的 `.mp` 内嵌有版本 1.0.2 的扩展资源，未改写程序内容。`.mp` 是此版本 Mind+ 实际保存出的项目格式。

## 更新分享包

在工作区根目录运行：

```powershell
python tools/package.py rotary-encoder
python tools/package.py potentiometer
```

生成最新源码 ZIP，并将 Mind+ 示例和分享说明复制到对应的 `releases/<传感器英文名>/`。日常修改示例请编辑 `sensors/<传感器英文名>/examples/MindPlus/` 中的文件，之后重新打包。

`.mpext` 由 Mind+ 导出；脚本不会伪造或自动改写它。修改库后，应从新路径刷新库、验证，再导出新的 .mpext 到对应发布文件夹。
