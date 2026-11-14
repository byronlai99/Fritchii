# 06 - 文件清单与提交索引

这份清单帮助未来对照新上游快速定位 Slim 改动点。若上游修改了文件名、目录或职责，应按功能寻找对应位置，不应机械套用旧路径。

## 1. 核心与文件系统

### 删除的文件

| 文件 | 用途 / 处理 |
|---|---|
| `src/core/crypto/key_manager.cpp` | KeyManager 实现，删除 |
| `src/core/crypto/key_manager.h` | KeyManager 声明，删除 |
| `src/core/crypto/partition_data_manager.cpp` | 旧密钥 / 分区数据管理层，删除 |
| `src/core/crypto/partition_data_manager.h` | 对应声明，删除 |
| `src/core/crypto/xts_encryption_layer.cpp` | 旧 XTS encryption wrapper，删除 |
| `src/core/crypto/xts_encryption_layer.h` | 对应声明，删除 |
| `src/core/file_sys/xts_archive.cpp` | 旧 XTS archive wrapper，删除 |
| `src/core/file_sys/xts_archive.h` | 对应声明，删除 |
| `src/core/loader/nax.cpp` | 旧 NAX loader，删除 |
| `src/core/loader/nax.h` | 对应声明，删除 |

### 保留并修改的文件

| 文件 | 修改方向 |
|---|---|
| `src/core/CMakeLists.txt` | 移除被删除文件的 source entries |
| `src/core/crypto/aes_util.h` | 定义通用 Key128 / Key256 |
| `src/core/crypto/aes_util.cpp` | 移除对 KeyManager header 的类型依赖 |
| `src/core/crypto/ctr_encryption_layer.h` | 改为 include aes_util.h |
| `src/core/crypto/ncz_decompressor.cpp` | 去除 KeyManager header 依赖 |
| `src/core/file_sys/decrypted_nca.cpp` | 删除外部 Title Key 设置；加入 encrypted NCA reject |
| `src/core/file_sys/content_archive.cpp/.h` | 增加 decrypted-only gate，去除 KeyManager include |
| `src/core/file_sys/fssystem/fssystem_aes_ctr_storage.h` | 改用通用 AES 类型头文件 |
| `src/core/file_sys/fssystem/fssystem_aes_xts_storage.h` | 改用通用 AES 类型头文件 |
| `src/core/file_sys/fssystem/fssystem_crypto_configuration.cpp` | 不再派生外部主机密钥；处理 ZeroKey |
| `src/core/file_sys/registered_cache.cpp/.h` | 移除 KeyManager include / 实例依赖 |
| `src/core/file_sys/sdmc_factory.cpp` | 移除 NAX 解密对象，交给 NCA 路径处理 |
| `src/core/file_sys/submission_package.cpp/.h` | 删除 NSP Ticket / SetTicketKeys |
| `src/core/loader/loader.cpp/.h` | 删除 NAX loader 类型和旧 key/NAX error 状态 |
| `src/core/loader/nsp.cpp` | 删除 KeyFileExists 等密钥前置检查 |
| `src/core/loader/xci.cpp` | 删除 KeyFileExists 等密钥前置检查 |

## 2. Common 与 Firmware

| 文件 | 修改方向 |
|---|---|
| `src/common/fs/fs_paths.h` | 删除 KeysDir 定义 |
| `src/common/fs/path_util.cpp` | 删除 KeysDir 路径初始化 |
| `src/common/fs/path_util.h` | 清理对应路径声明 |
| `src/common/settings.h` | 删除 use_dev_keys 设置 |
| `src/frontend_common/content_manager.h` | 删除 AreKeysPresent 等密钥存在检查 |
| `src/frontend_common/firmware_manager.cpp/.h` | 删除 InstallKeys / KeyInstallResult 及密钥安装前置流程；保留固件检查 |

## 3. Desktop Qt

| 文件 | 修改方向 |
|---|---|
| `src/qt_common/qt_string_lookup.h` | 清理旧密钥 / NAX loader 文案映射 |
| `src/qt_common/util/content.cpp/.h` | 删除 Qt key import utility |
| `src/yuzu/main.ui` | 删除 Install Decryption Keys 菜单 QAction |
| `src/yuzu/main_window.cpp/.h` | 删除 key menu handler、启动 key check 和 dev key reload；启动继续设置 firmware version |
| `src/yuzu/fritchii/whats-new.md` | 新增文件；修复 fritchii.qrc 对资源文件的引用 |

## 4. Android / JNI / strings

| 文件 / 目录 | 修改方向 |
|---|---|
| `src/android/app/src/main/java/org/yuzu/yuzu_emu/NativeLibrary.kt` | 删除 reloadKeys / installKeys / areKeysPresent |
| `src/android/app/src/main/java/org/yuzu/yuzu_emu/fragments/InstallableFragment.kt` | 删除 console / Amiibo key install UI |
| `src/android/app/src/main/java/org/yuzu/yuzu_emu/fragments/SetupFragment.kt` | 删除 prod.keys setup wizard / checks |
| `src/android/app/src/main/java/org/yuzu/yuzu_emu/model/HomeViewModel.kt` | 删除 checkKeys 状态 |
| `src/android/app/src/main/java/org/yuzu/yuzu_emu/ui/main/MainActivity.kt` | 删除 key picker / import handlers |
| `src/android/app/src/main/java/org/yuzu/yuzu_emu/utils/InstallableActions.kt` | 删除 processKey |
| `src/android/app/src/main/jni/native.cpp` | 删除对应 JNI key APIs |
| `src/android/app/src/main/res/drawable/ic_key.xml` | 删除只用于密钥安装界面的图标 |
| `src/android/app/src/main/res/values*/strings.xml` | 清理已废弃的密钥安装文案，更新保留的 decrypted-only 说明 |

## 5. Workflow

| 文件 | 修改方向 |
|---|---|
| `.github/workflows/renhdrum-v584-build.yml` | 加入 Slim branch build 条件、独立 build tag 和独立 artifact 命名 |

## 6. 22 个 Slim 实施提交

以下列表从旧到新排列。可用于查找原始改动，但未来移植时仍需先对照新上游，不要无条件整串 cherry-pick。

| # | Commit | 内容 |
|---:|---|---|
| 01 | `c99eb99` | Begin slim keyless core：拒绝加密 NCA，移除 Ticket 派生 |
| 02 | `422ba9e` | 移除 NSP Ticket 处理和 SD NAX key dependency |
| 03 | `320d7c8` | 删除旧加密 NAX loading |
| 04 | `c66603d` | 移除 frontend key import 和 firmware key prerequisites |
| 05 | `46833b3` | 移除 Desktop key prompts、菜单、dev keys 和 keys directory |
| 06 | `7969f20` | 移除 Android prod.keys setup、prompts 和 JNI key APIs |
| 07 | `1652381` | 删除旧 KeyManager 和 console key derivation sources |
| 08 | `84bfa87` | 清理旧 key / NAX loader errors 和 hints |
| 09 | `bd2c198` | 删除未使用的 XTS archive decryption wrapper |
| 10 | `90a331c` | 移除 Android console 和 Amiibo key import surfaces |
| 11 | `288db43` | 清理第一批 Android 旧 key setup strings |
| 12 | `404e6db` | 清理第二批 Android 旧 key setup strings |
| 13 | `ddc1329` | 清理第三批 Android 旧 key setup strings |
| 14 | `d337a45` | 清理残留 Android key strings，加入 decrypted-only 指引 |
| 15 | `4139f8d` | 移除 Qt key installation result strings |
| 16 | `9cf611f` | 允许 Fritchii-hdr-slim 触发 Windows MSVC test build |
| 17 | `7796cdf` | 清理无用 Android key-install imports |
| 18 | `8c48a46` | 解耦 Fssystem AES storage 与 KeyManager |
| 19 | `60e7eb7` | 将通用 AES 类型依赖移出 KeyManager |
| 20 | `44f9847` | 去重 / 调整 KeyManager 删除后的 AES utility includes |
| 21 | `d6cbe5e` | 增加 Slim branch release notes resource |
| 22 | `a1fa4f0` | 移除 NSP / XCI 最后的 KeyManager checks |

完整 SHA 可在 Git 历史中查看；表中短 SHA 用于快速定位。

## 7. 本次提交边界

- Slim 实施开始点：`9dcd5e280f272671b076ecad476da1e2216954ff`
- Slim 实施末端：`a1fa4f0b1988aee5650c052411a840264b01f2e9`
- 这 22 个提交基于原 feature baseline 叠加。
- 更早的 README / CONTRIBUTING 清理提交不属于这 22 个 Slim 代码实施提交。
- 当前工作流失败记录见 [04 - 构建与验证](04-build-validation.md)。
