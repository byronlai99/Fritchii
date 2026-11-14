# 01 - 核心密钥与密码层

本模块处理的是“外部主机密钥管理”和“模拟器内部通用密码功能”之间的边界。这是未来移植时最容易误删、也最值得单独提交的部分。

## 1. 目标边界

移除：

- KeyManager 单例及其密钥文件扫描、解析、派生和重载逻辑；
- prod.keys / title.keys 等外部密钥文件的依赖；
- Ticket / Title Key / Titlekek 等零售内容解密设置路径；
- 只为旧 KeyManager / NAX / XTS archive 包装器服务的代码。

保留：

- 通用 AES 类型和 AES 实现；
- 已解密内容读取仍然会用到的 CTR 等文件访问支持；
- 游戏运行和模拟器系统服务真正依赖的内部密码学实现；
- Fssystem 处理已解密 NCA 所需的数据结构与文件系统逻辑。

## 2. 已实施的核心改动

### A. 删除旧 KeyManager 及专用管理层

删除文件：

- `src/core/crypto/key_manager.cpp`
- `src/core/crypto/key_manager.h`
- `src/core/crypto/partition_data_manager.cpp`
- `src/core/crypto/partition_data_manager.h`

同时从 `src/core/CMakeLists.txt` 移除对应源文件条目。

KeyManager 原先承担的工作不只是“读取一个 keys 文件”，还涉及多类密钥索引、密钥派生、Ticket、Title Key、密钥文件路径和 reload 等职责。未来上游如果重构 KeyManager，不要照着旧文件名机械删除；应重新追踪调用链，确认新版本中这些职责被拆到了哪些类。

**Amiibo 专用密钥是独立例外：** NFC Amiibo 加解密使用 `key_retail.bin`，它不是主机 `prod.keys` / `title.keys`，也不参与 NCA、BIS 或 Ticket 密钥派生。为兼容旧用户数据，`amiibo_crypto.cpp` 仍读取 `<EdenDir>/keys/key_retail.bin`；这只是保留旧文件位置，不会恢复通用 `KeysDir` 枚举，也不会自动创建通用密钥目录。没有该文件时，现有 NFC 逻辑仍可按无密钥只读模式载入加密 Amiibo。

### B. 删除旧 XTS / NAX 专用包装层

删除文件：

- `src/core/crypto/xts_encryption_layer.cpp`
- `src/core/crypto/xts_encryption_layer.h`
- `src/core/file_sys/xts_archive.cpp`
- `src/core/file_sys/xts_archive.h`

NAX loader 的删除记录见 [02 - 内容加载与解密边界](02-content-loading.md)。

注意：删除这两个旧包装层**不等于**删除所有 AES-XTS 实现。当前 `fssystem_aes_xts_storage.h` 仍然存在，只是它不再通过 KeyManager 头文件取得通用类型。

### C. 将通用 AES key 类型从 KeyManager 解耦

在 `src/core/crypto/aes_util.h` 中保留通用类型：

- `Key128`
- `Key256`

原本依赖 `key_manager.h` 才能取得这些类型的文件，改为 include `aes_util.h`。本次涉及：

- `src/core/crypto/aes_util.cpp`
- `src/core/crypto/ctr_encryption_layer.h`
- `src/core/file_sys/fssystem/fssystem_aes_ctr_storage.h`
- `src/core/file_sys/fssystem/fssystem_aes_xts_storage.h`
- `src/core/crypto/ncz_decompressor.cpp)

以后上游升级若改了 AES 模板、Key 类型定义或 include 结构，应先恢复这种依赖方向：**通用 AES 类型不应该依赖 KeyManager**。不要为了消除 include 而把所有密码代码删掉。

### D. 禁止从 Fssystem 派生外部主机密钥

文件：

- `src/core/file_sys/fssystem/fssystem_crypto_configuration.cpp`

当前实现使用 `RejectEncryptedKey` 回调，不再执行外部密钥派生。对 `KeyType::ZeroKey` 有特殊处理，输出零值；其他需要外部密钥的类型以失败关闭方式处理。

这是兼容已解密 NCA 读取的边界实现，不是可通用解密加密 NCA 的替代方案。若新上游改变 `KeyType`、回调签名或 Fssystem 配置结构，应重新核对：

1. 零密钥是否仍是合法的独立情况；
2. 加密 header / section 是否会调用其他密钥回调；
3. 错误是否能向上返回，而不是继续使用伪造密钥；
4. 已解密 NCA 的读取路径是否仍然正常。

### E. 已解密 NCA 不设置外部 Title Key

文件：

- `src/core/file_sys/decrypted_nca.cpp`
- `src/core/file_sys/content_archive.cpp`

已经删除原先通过 KeyManager 取得 Title Key / Titlekek 并设置外部解密 key 的处理。改为识别已解密 NCA，并拒绝加密 NCA。具体加载行为见模块 02。

### F. BIS 分区读取与外部密钥派生分离

文件：

- `src/core/file_sys/bis_factory.cpp`

`BISFactory::OpenPartitionStorage()` 不再依赖已删除的 `PartitionDataManager` 或外部 `KeyManager`：

- 六种 Package2 storage 只按 NAND 中的文件名读取原始文件，不触发密钥派生或解密。
- CalibrationBinary 代表解密后的 PRODINFO 视图，需要外部 BIS key；Slim 没有密钥派生组件，因此明确返回空文件，不会把加密 PRODINFO 当作明文返回。

这不影响 `OpenPartition()` 提供的目录型 BIS 分区访问（例如 `/system`、`/user`）。分区定位/原始读取留在 BIS 层，外部密钥派生不再隐式耦合到打开分区的过程。

## 3. 当前核心文件地图

| 文件 | 当前处理 |
|---|---|
| `src/core/crypto/key_manager.cpp/.h` | 删除 |
| `src/core/crypto/partition_data_manager.cpp/.h` | 删除 |
| `src/core/crypto/xts_encryption_layer.cpp/.h` | 删除 |
| `src/core/file_sys/xts_archive.cpp/.h` | 删除 |
| `src/core/crypto/aes_util.h` | 保留；提供通用 Key128 / Key256 |
| `src/core/crypto/aes_util.cpp` | 保留；解除 KeyManager include 依赖 |
| `src/core/crypto/ctr_encryption_layer.h` | 保留；改用通用 AES 头文件 |
| `src/core/file_sys/fssystem/fssystem_aes_ctr_storage.h` | 保留；改用通用 AES 头文件 |
| `src/core/file_sys/fssystem/fssystem_aes_xts_storage.h` | 保留；改用通用 AES 头文件 |
| `src/core/file_sys/fssystem/fssystem_crypto_configuration.cpp` | 保留；拒绝外部密钥派生 |
| `src/core/CMakeLists.txt` | 移除已经删除的源文件条目 |

## 4. 上游移植建议顺序

1. 对新版本执行全仓库 KeyManager / Key 类型 / 密钥派生引用扫描。
2. 找到新版本通用 AES 类型定义的最佳位置，先解耦类型和 include。
3. 再替换 NCA / Fssystem 的外部密钥回调。
4. 清除 Ticket / Title Key 的调用者。
5. 删除不再有调用者的专用源文件，并同步更新 CMake。
6. 编译 core 目标，解决 include、模板类型和链接错误。
7. 最后才删除其他 frontend 的密钥 API；不要让 UI 层先于核心接口变化造成大量无意义冲突。

## 5. 验收检查

- [ ] `KeyManager` 不再从运行时代码中引用。
- [ ] 不再扫描、读取或重载 prod.keys / title.keys。
- [ ] 没有残留的 Ticket / Title Key 派生调用。
- [ ] `Key128` / `Key256` 不依赖已删除的头文件。
- [ ] CMake 不再引用已删除的 .cpp。
- [ ] 通用 AES、CTR、必要的 XTS storage 和系统服务功能没有被误删。
- [ ] 加密 NCA 走明确错误路径。
- [ ] 已解密 NCA 的打开路径保留。
