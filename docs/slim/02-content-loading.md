# 02 - 内容加载与解密边界

本模块规定 Slim 接受哪些游戏内容，以及旧的 NAX / NSP Ticket / NCA 解密路径如何处理。

## 1. 当前目标行为

- 已解密 NCA：允许进入后续文件系统解析。
- 加密 NCA：明确拒绝，不尝试外部密钥派生。
- 已解密 NUP 或提取后的游戏目录：作为当前主要目标输入。
- ExeFS / RomFS：保留它们作为已解密游戏内容的组成部分。
- NAX：不再走 SD 卡 NAX 解密流程。
- NSP / XCI：当前容器读取代码仍存在；不能把本阶段描述为已经完全删除 NSP / XCI loader。

## 2. 已实施改动

### A. NCA 解密边界

文件：

- `src/core/file_sys/decrypted_nca.cpp`
- `src/core/file_sys/content_archive.cpp`
- `src/core/file_sys/fssystem/fssystem_crypto_configuration.cpp`

核心行为：

1. `OpenDecryptedNca` 检查 NCA header magic。
2. 已解密 magic（`NcaHeader::MagicDecrypted`）允许继续处理。
3. 非解密 NCA 进入 `RejectEncryptedNca`，返回明确的“不支持加密 NCA，请使用已解密 NUP 或游戏目录”错误。
4. `NCA` content archive 的构造/打开路径也增加 decrypted gate，避免调用者绕过前面的检查。
5. Fssystem 不再为加密 header / section 派生外部主机密钥。

未来上游若重构 NCA 打开函数，要确认 gate 放在所有入口都无法绕过的位置，而不是只放在某个 UI 启动流程里。

### B. 删除 NAX loader 与 SD NAX 解密包装路径

删除文件：

- `src/core/loader/nax.cpp`
- `src/core/loader/nax.h`

调整文件：

- `src/core/CMakeLists.txt`
- `src/core/loader/loader.cpp`
- `src/core/loader/loader.h`
- `src/core/file_sys/sdmc_factory.cpp`

SD 注册内容不再构造旧的 NAX XTS archive 解密对象，而是把文件交给普通 NCA 解析及 decrypted-only 检查。旧 NAX loader 状态码、提示和路径识别也被清理。

### C. NSP Ticket 处理移除

文件：

- `src/core/file_sys/submission_package.cpp`
- `src/core/file_sys/submission_package.h`

改动包括：

- 不再从 NSP 文件集合中提取 `.tik` 文件；
- 删除 `SetTicketKeys`；
- 删除 NSP 中持有 KeyManager 的依赖；
- 不再从 Ticket 中取得 Rights ID 对应的 Title Key；
- Ticket 文件被忽略，因为当前分支不支持加密 package 内容。

### D. 清理 loader 错误状态与 UI 提示

文件：

- `src/core/loader/loader.cpp`
- `src/core/loader/loader.h`
- `src/qt_common/qt_string_lookup.h`

删除只服务于外部密钥缺失或旧 NAX 解密失败的错误状态和文字。遇到不支持的加密内容时，应统一走当前仍然存在的通用错误路径，并提供用户可理解的说明。

## 3. 尚未完成的容器层工作

当前仍存在以下文件：

- `src/core/loader/nsp.cpp`
- `src/core/loader/xci.cpp`
- `src/core/file_sys/submission_package.cpp/.h`
- `src/core/file_sys/decrypted_nca.cpp` 中的 package / gamecard 处理函数

因此当前阶段应描述为：

> NSP / XCI 的容器读取代码仍然存在，但里面的加密 NCA 内容不应再获得外部密钥解密能力；实际可用性取决于容器中内容是否已经解密，以及 NCA gate 是否覆盖所有入口。

如果未来决定进一步缩小 Slim：

1. 先确认 NUP、已解密目录是否已经覆盖 OoT 的实际加载需求；
2. 用调用图确认 NSP / XCI loader 是否仍有必要；
3. 单独移除 NSP / XCI 的 loader 注册和容器依赖；
4. 同步处理游戏列表识别、安装流程、文件关联、错误状态和 UI；
5. 不要在没有确认调用者之前直接删除 `content_archive` 或通用 NCA 文件系统代码。

## 4. 当前保留与删除的文件地图

| 文件 / 组件 | 当前处理 |
|---|---|
| `src/core/loader/nax.cpp/.h` | 删除 |
| `src/core/file_sys/xts_archive.cpp/.h` | 删除 |
| `src/core/crypto/xts_encryption_layer.cpp/.h` | 删除 |
| `src/core/file_sys/sdmc_factory.cpp` | 不再走 NAX 解密包装路径 |
| `src/core/file_sys/submission_package.cpp/.h` | 删除 Ticket key 设置 |
| `src/core/file_sys/decrypted_nca.cpp` | 保留；作为已解密内容处理和加密 NCA 拒绝入口 |
| `src/core/file_sys/content_archive.cpp/.h` | 保留；增加加密 NCA gate |
| `src/core/loader/nsp.cpp` | 保留；仍有容器 loader |
| `src/core/loader/xci.cpp` | 保留；仍有容器 loader |
| `src/core/file_sys/fssystem/fssystem_crypto_configuration.cpp` | 保留；拒绝外部密钥派生 |

## 5. 验收检查

- [ ] 加密 NCA 无法进入正常挂载/读取流程。
- [ ] 已解密 NCA 可以继续解析。
- [ ] NAX 不再通过旧 loader 或 SD XTS archive 解密路径加载。
- [ ] NSP Ticket 不会被导入或设置为 Title Key。
- [ ] 错误信息准确，不再要求用户安装 prod.keys。
- [ ] NUP / 提取目录启动路径保留。
- [ ] ExeFS / RomFS 读取功能保留。
- [ ] 明确记录 NSP / XCI loader 是否仍需要；未做决定前不要宣称已完全移除。
