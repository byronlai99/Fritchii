<!--
# SPDX-FileCopyrightText: Copyright 2026 Fritchii-HDrum contributors
# SPDX-License-Identifier: GPL-3.0-or-later

# SPDX-FileCopyrightText: 2018 yuzu Emulator Project
# SPDX-License-Identifier: GPL-2.0-or-later
-->

# Fritchii-HDrum

**Fritchii-HDrum 中文说明**

**Fritchii-HDrum** is an independent Windows x64 build of [Fritchii](https://github.com/Frizti/Fritchii) with [RenHDrum](https://github.com/trzz1214/RenHDrum) integration for enhanced controller rumble and haptics.

**Fritchii-HDrum** 是一个独立的 Windows x64 版本，基于 [Fritchii](https://github.com/Frizti/Fritchii)，并整合了 [RenHDrum](https://github.com/trzz1214/RenHDrum)，以增强控制器的震动与触觉反馈功能。

This is a personal, experimental fork. It is not an official release of Frizti's Fritchii project and is not affiliated with Nintendo, Sony or Microsoft.

这是一个个人维护的实验性分支，并非 Frizti 的 Fritchii 项目的官方版本，也与 Nintendo、Sony 或 Microsoft 没有任何隶属关系。

## Downloads

## 下载

Download the latest Windows x64 build from the [Fritchii-HDrum Releases](https://github.com/byronlai99/Fritchii-HDrum/releases) page.

请前往 [Fritchii-HDrum Releases 发布页面](https://github.com/byronlai99/Fritchii-HDrum/releases) 下载最新的 Windows x64 版本。

1. Download and extract the complete ZIP archive.
   下载并解压完整的 ZIP 压缩包。
2. Keep the included DLL files in the same folder as the emulator.
   将压缩包内附带的 DLL 文件与模拟器放在同一个文件夹中。
3. Run `lichii.exe`.
   运行 `lichii.exe`。

The Windows release workflow enables RenHDrum and packages its runtime with the application. If RenHDrum cannot be loaded or a controller-specific haptics path is unavailable, the emulator can fall back to standard rumble.

Windows 发布工作流程会启用 RenHDrum，并将其运行时文件与应用程序一并打包。如果无法加载 RenHDrum，或某种控制器专用的触觉反馈路径不可用，模拟器可以回退到标准震动模式。

Games, keys and firmware are not included. Only use games and system files you have legally obtained.

本项目不包含游戏、密钥或固件。请仅使用通过合法途径取得的游戏和系统文件。

## HD Rumble settings

## HD Rumble 设置

To open the HD Rumble settings:

打开 HD Rumble 设置的路径：

**Configure (Ctrl + ,) → Controls → Configure (under Vibration) → HD Rumble**

**配置（Ctrl + ,）→ 控制器 → 配置（震动选项下方）→ HD Rumble**

The HD Rumble page provides controller-specific options. Available controls depend on the connected controller and its connection type.

HD Rumble 页面提供针对不同控制器的选项。实际显示的控制项取决于已连接的控制器型号及其连接方式。

## Controller rumble and haptics support

## 控制器震动与触觉反馈支持

Fritchii-HDrum combines the emulator's built-in controller drivers with RenHDrum for enhanced vibration.

Fritchii-HDrum 将模拟器内置的控制器驱动与 RenHDrum 相结合，以提供增强的震动效果。

| Controller | Rumble / haptics support | Notes |
|---|---|---|
| Nintendo Switch Pro Controller | HD Rumble through RenHDrum | Preserves high- and low-frequency vibration data. |
| Nintendo Joy-Con (L/R) | HD Rumble through the built-in Joy-Con driver | The built-in Joy-Con driver handles these controllers by default. |
| Sony DualSense | Advanced haptics | USB uses the haptic actuators through multi-channel audio; Bluetooth uses HID haptics when supported by the RenHDrum runtime. |
| Sony DualSense Edge | Advanced haptics | USB and Bluetooth haptics paths are implemented, subject to runtime and device availability. |
| Xbox One controllers | Enhanced rumble | RenHDrum processing; trigger rumble is available only where the controller and connection expose trigger motors. |
| Xbox Series X\|S controllers | Enhanced rumble | RenHDrum processing; trigger rumble is available only where the controller and connection expose trigger motors. |
| Xbox 360-compatible controllers | Enhanced rumble | Supported when SDL identifies the controller as an Xbox 360-style gamepad. |
| Compatible third-party XInput controllers | Enhanced rumble | Includes supported Xbox-style devices from brands such as GameSir, 8BitDo and PowerA. Actual features vary by model and connection. |

| 控制器 | 震动 / 触觉反馈支持 | 备注 |
|---|---|---|
| Nintendo Switch Pro Controller | 通过 RenHDrum 提供 HD Rumble | 保留高频与低频震动数据。 |
| Nintendo Joy-Con (L/R) | 通过内置 Joy-Con 驱动提供 HD Rumble | 默认由模拟器内置的 Joy-Con 驱动处理这些控制器。 |
| Sony DualSense | 进阶触觉反馈 | USB 通过多声道音频驱动触觉执行器；在 RenHDrum 运行时支持的情况下，蓝牙使用 HID 触觉反馈。 |
| Sony DualSense Edge | 进阶触觉反馈 | 已实现 USB 与蓝牙触觉反馈路径，实际可用性取决于运行时和设备支持情况。 |
| Xbox One 控制器 | 增强震动 | 由 RenHDrum 处理；只有当控制器和连接方式提供扳机马达时，才可使用扳机震动。 |
| Xbox Series X\\|S 控制器 | 增强震动 | 由 RenHDrum 处理；只有当控制器和连接方式提供扳机马达时，才可使用扳机震动。 |
| Xbox 360 兼容控制器 | 增强震动 | 当 SDL 将控制器识别为 Xbox 360 类型手柄时即可支持。 |
| 兼容的第三方 XInput 控制器 | 增强震动 | 包括 GameSir、8BitDo 和 PowerA 等品牌的受支持 Xbox 风格设备。实际功能因型号和连接方式而异。 |

### Controller notes

### 控制器注意事项

- **Joy-Con:** The built-in Joy-Con driver is enabled by default and takes priority for Joy-Con devices. Their rumble is handled by that native driver rather than the RenHDrum path.
  - **Joy-Con：** 内置 Joy-Con 驱动默认启用，并优先处理 Joy-Con 设备。Joy-Con 的震动由该原生驱动负责，而不是通过 RenHDrum 路径处理。
- **DualSense over Bluetooth:** Advanced haptics require a compatible RenHDrum runtime with the Bluetooth HID report function and a usable HID device path. If unavailable, the emulator falls back to standard rumble.
  - **DualSense 蓝牙连接：** 进阶触觉反馈需要兼容的 RenHDrum 运行时，具备蓝牙 HID 报告功能以及可用的 HID 设备路径。如果这些条件不满足，模拟器会回退到标准震动。
- **Xbox trigger rumble:** Trigger motors work only on controllers and connection modes that expose them to SDL. Many wireless receivers expose only the main grip motors.
  - **Xbox 扳机震动：** 只有当控制器和连接模式向 SDL 提供扳机马达时，该功能才能使用。许多无线接收器只会提供手柄主体的震动马达。
- **Nintendo Switch 2 Pro Controller:** Native Switch 2 HD Rumble support is not implemented in the current build. It may still be detected by Windows or SDL, but this build does not include the dedicated Switch 2 HD Rumble driver.
  - **Nintendo Switch 2 Pro Controller：** 当前版本尚未实现原生 Switch 2 HD Rumble 支持。Windows 或 SDL 仍可能识别到该控制器，但此版本并未包含专用的 Switch 2 HD Rumble 驱动。

Controller behavior depends on the game, controller firmware, operating system, SDL device detection and whether the controller is connected over USB or Bluetooth. Not every game uses HD Rumble or advanced haptics.

控制器的实际表现取决于游戏本身、控制器固件、操作系统、SDL 的设备识别结果，以及控制器使用 USB 还是蓝牙连接。并非所有游戏都会使用 HD Rumble 或进阶触觉反馈功能。

## Saves and configuration

## 存档与配置

Fritchii-HDrum uses separate save-data and configuration locations from Lichii and other emulator forks. Existing saves and settings from those installations remain separate and are not automatically shared.

Fritchii-HDrum 使用独立于 Lichii 及其他模拟器分支的存档数据与配置文件路径。其他版本中的现有存档和设置会保持独立，不会自动共享。

Keep backups of important save data before testing experimental builds. Do not copy configuration files between emulator forks without checking their compatibility.

在测试实验性版本之前，请先备份重要存档。未经兼容性确认，请勿在不同模拟器分支之间直接复制配置文件。

## Project and credits

## 项目与致谢

- **Fritchii:** Based on [Frizti/Fritchii](https://github.com/Frizti/Fritchii) and its contributors.
  - **Fritchii：** 本项目基于 [Frizti/Fritchii](https://github.com/Frizti/Fritchii) 开发，并承蒙其贡献者的工作成果。
- **RenHDrum:** Enhanced controller rumble and haptics by [Three-Dola](https://github.com/trzz1214/RenHDrum).
  - **RenHDrum：** 由 [Three-Dola](https://github.com/trzz1214/RenHDrum) 开发的控制器增强震动与触觉反馈组件。
- **Upstream foundations:** This project inherits code and contributions from the emulator projects in its development history. Original copyright notices and third-party licenses are retained.
  - **上游项目基础：** 本项目继承了开发历程中相关模拟器项目的代码与贡献，并保留原有版权声明及第三方许可证。

Fritchii-HDrum is experimental software. Compatibility, performance and controller behavior may vary. When reporting an issue, include the exact build, controller model, connection type, relevant settings and a log from the affected session. Check logs for personal information before sharing them.

Fritchii-HDrum 属于实验性软件，兼容性、性能和控制器表现可能因环境而异。提交问题报告时，请提供准确的版本编号、控制器型号、连接方式、相关设置，以及出现问题时的运行日志。分享日志之前，请先检查其中是否包含个人信息。

See [LICENSE.txt](LICENSE.txt) and the source files for applicable license terms.

有关适用的许可证条款，请参阅 [LICENSE.txt](LICENSE.txt) 及源代码文件。
