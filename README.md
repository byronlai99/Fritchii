<!--
# SPDX-FileCopyrightText: Copyright 2026 Fritchii-HDrum contributors
# SPDX-License-Identifier: GPL-3.0-or-later

# SPDX-FileCopyrightText: 2018 yuzu Emulator Project
# SPDX-License-Identifier: GPL-2.0-or-later
-->

# Fritchii-HDrum

**Fritchii-HDrum** is an independent Windows x64 build of [Fritchii](https://github.com/Frizti/Fritchii) with [RenHDrum](https://github.com/trzz1214/RenHDrum) integration for enhanced controller rumble and haptics.

This is a personal, experimental fork. It is not an official release of Frizti's Fritchii project and is not affiliated with Nintendo, Sony or Microsoft.

## Downloads

Download the latest Windows x64 build from the [Fritchii-HDrum Releases](https://github.com/byronlai99/Fritchii-HDrum/releases) page.

1. Download and extract the complete ZIP archive.
2. Keep the included DLL files in the same folder as the emulator.
3. Run `lichii.exe`.

The Windows release workflow enables RenHDrum and packages its runtime with the application. If RenHDrum cannot be loaded or a controller-specific haptics path is unavailable, the emulator can fall back to standard rumble.

Games, keys and firmware are not included. Only use games and system files you have legally obtained.

## HD Rumble settings

To open the HD Rumble settings:

**Configure (Ctrl + ,) → Controls → Configure (under Vibration) → HD Rumble**

The HD Rumble page provides controller-specific options. Available controls depend on the connected controller and its connection type.

## Controller rumble and haptics support

Fritchii-HDrum combines the emulator's built-in controller drivers with RenHDrum for enhanced vibration.

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

### Controller notes

- **Joy-Con:** The built-in Joy-Con driver is enabled by default and takes priority for Joy-Con devices. Their rumble is handled by that native driver rather than the RenHDrum path.
- **DualSense over Bluetooth:** Advanced haptics require a compatible RenHDrum runtime with the Bluetooth HID report function and a usable HID device path. If unavailable, the emulator falls back to standard rumble.
- **Xbox trigger rumble:** Trigger motors work only on controllers and connection modes that expose them to SDL. Many wireless receivers expose only the main grip motors.
- **Nintendo Switch 2 Pro Controller:** Native Switch 2 HD Rumble support is not implemented in the current build. It may still be detected by Windows or SDL, but this build does not include the dedicated Switch 2 HD Rumble driver.

Controller behavior depends on the game, controller firmware, operating system, SDL device detection and whether the controller is connected over USB or Bluetooth. Not every game uses HD Rumble or advanced haptics.

## Saves and configuration

Fritchii-HDrum uses separate save-data and configuration locations from Lichii and other emulator forks. Existing saves and settings from those installations remain separate and are not automatically shared.

Keep backups of important save data before testing experimental builds. Do not copy configuration files between emulator forks without checking their compatibility.

## Project and credits

- **Fritchii:** Based on [Frizti/Fritchii](https://github.com/Frizti/Fritchii) and its contributors.
- **RenHDrum:** Enhanced controller rumble and haptics by [Three-Dola](https://github.com/trzz1214/RenHDrum).
- **Upstream foundations:** This project inherits code and contributions from the emulator projects in its development history. Original copyright notices and third-party licenses are retained.

Fritchii-HDrum is experimental software. Compatibility, performance and controller behavior may vary. When reporting an issue, include the exact build, controller model, connection type, relevant settings and a log from the affected session. Check logs for personal information before sharing them.

See [LICENSE.txt](LICENSE.txt) and the source files for applicable license terms.
