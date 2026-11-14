# Fritchii-HDrum Slim：设计与移植记录

> 本目录是 Slim 分支的工程记录和上游版本移植手册。它记录的是已经实际落到代码中的改动，不代表这些改动已经全部编译通过或达到可发布状态。

## 1. 目标

Slim 的目标是让模拟器优先运行**已经解密的游戏内容**，并移除外部主机密钥管理及其用户界面、安装流程和启动检查。

当前设计方向：

1. 优先保证 Ocarina of Time（OoT）已解密内容的启动与运行。
2. 保留已解密 NUP、提取后的游戏目录，以及运行这些内容所需的 ExeFS、RomFS 和模拟器系统服务。
3. 不再提供 prod.keys、title.keys、Ticket、Title Key 或主机密钥派生流程。
4. 对加密 NCA 内容明确失败关闭，不尝试从用户密钥文件中恢复解密能力。
5. 不做“删除所有加密算法”的改造。游戏运行、文件系统和系统服务仍可能需要通用 AES 或其他内部密码功能。

## 2. 当前分支基线

| 项目 | 记录 |
|---|---|
| 仓库 | `byronlai99/Fritchii-HDrum` |
| 工作分支 | `Fritchii-hdr-slim` |
| 原始基础分支 | `feature/v5.8.4-renhdrum-zh-cn` |
| Slim 改造基线提交 | `9dcd5e280f272671b076ecad476da1e2216954ff` |
| 22 个 Slim 实施提交的末端 | `a1fa4f0b1988aee5650c052411a840264b01f2e9` |
| 对应上游版本基线 | Fritchii 5.8.4 衍生代码 |

上述末端 SHA 是这份实施记录的代码快照锚点。后续文档提交会让分支 HEAD 向前移动，但不改变原先 22 个 Slim 实施提交的范围。

## 3. 实施状态

| 模块 | 当前记录 | 状态 |
|---|---|---|
| 外部密钥管理核心 | 删除 KeyManager 及部分专用密钥派生/解密包装层；拆开通用 AES 类型依赖 | 已实施，未完成编译验证 |
| NCA 解密边界 | 已加入仅接受解密 NCA 的检查；加密 NCA 明确拒绝 | 已实施，未完成编译验证 |
| NAX / SD 注册内容 | 删除 NAX loader 和 XTS archive 包装路径，SD 注册内容不再走 NAX 解密 | 已实施，未完成编译验证 |
| NSP Ticket | 移除 Ticket 文件导入和 Title Key 设置流程 | 已实施，未完成编译验证 |
| 桌面 Qt 前端 | 移除密钥菜单、提示、启动检查、keys 目录初始化和相关设置 | 已实施，未完成编译验证 |
| Android 前端 | 移除 prod.keys / Amiibo key 安装界面、JNI API、状态检查和相关翻译文案 | 已实施；Android 构建未验证 |
| Firmware 前端流程 | 去掉外部密钥安装前置条件；保留固件存在性/版本检查相关功能 | 已实施，未完成编译验证 |
| 构建工作流 | 增加 Slim 分支的 Windows MSVC 测试构建识别及独立构建标签 | 已实施；最新构建结果需以 Actions 为准 |
| 完全移除 NSP / XCI 容器路径 | NSP / XCI loader 和容器读取代码仍存在；目前依赖 NCA 解密边界拒绝加密内容 | 未完成 |
| 全仓库死代码清理 | 尚未进行完整清理 | 未完成 |
| 运行时回归 | OoT、RomFS、存档、控制器、RenHDrum 等完整测试尚未完成 | 未完成 |

**不要把“密钥 UI 已删除”理解为“所有加密游戏路径已经从仓库彻底删除”。** 当前更准确的描述是：外部密钥流程已大幅切断；NCA 层加入 decrypted-only gate；部分旧 NAX / XTS 包装层已删除；NSP / XCI 容器读取代码仍在。

## 4. 模块文档

- [01 - 核心密钥与密码层](01-core-crypto.md)：KeyManager 删除范围、AES 类型解耦、必须保留的内部密码功能。
- [02 - 内容加载与解密边界](02-content-loading.md)：NCA、NAX、NSP、XCI、Ticket 和 SD 注册内容的处理方式。
- [03 - 桌面与 Android 前端](03-frontends.md)：Qt、Android、JNI、Firmware Manager 和翻译文案的改动。
- [04 - 构建与验证](04-build-validation.md)：CI 调整、已知失败原因、静态扫描和回归测试清单。
- [05 - 上游升级移植流程](05-upstream-porting.md)：上游新版本出现后，如何逐模块重做或移植 Slim。
- [06 - 文件清单与提交索引](06-file-map-and-commits.md)：改动文件地图和 22 个 Slim 实施提交记录。

## 5. 核心设计规则

### 保留通用密码功能

Slim 不是把 `src/core/crypto` 整个删除。不要因为删除了 KeyManager，就批量删除 AES、CTR、XTS、RSA、SHA 或 OpenSSL 相关代码。必须逐个追踪调用者，并区分：

- 外部主机密钥来源、Ticket、Title Key、密钥派生及零售内容解密路径；
- 游戏运行、系统服务、已解密内容读取仍然使用的通用密码原语。

### 失败关闭

遇到加密 NCA 或必须依赖外部主机密钥的内容时，应给出清晰错误并停止，不应悄悄尝试其他密钥路径，也不应生成看似有效但内容错误的文件。

### 模块化提交

未来每次移植应按模块分别提交：核心依赖、内容 gate、NAX / Ticket、桌面前端、Android 前端、翻译、构建配置、测试。不要把一次上游同步和所有 Slim 修改混成一个巨大提交。

## 6. 版本升级时的基本顺序

1. 固定新上游版本和提交 SHA，建立新的移植分支。
2. 对照本目录的文件地图，确认上游文件是否改名、拆分或合并。
3. 先移植核心密钥边界和通用 AES 类型解耦。
4. 再移植 NCA gate、NAX / Ticket / 容器行为。
5. 再移植 Desktop Qt、Android、JNI 和翻译清理。
6. 检查 CMake、资源文件和 CI 工作流。
7. 完成静态扫描、Windows x64 MSVC 构建，再进行功能回归。

详细操作见 [05 - 上游升级移植流程](05-upstream-porting.md)。

## 7. 当前发布限制

本分支目前是实验性代码快照，不应仅凭 UI 已经不显示密钥导入，或 CMake 配置成功，就视为可发布。最低发布门槛是：

- Windows x64 MSVC 完整编译成功；
- 运行时确认加密 NCA 被明确拒绝；
- 确认已解密 NUP / 游戏目录能正常启动；
- 确认 ExeFS、RomFS、存档、控制器输入和 RenHDrum 没有回归；
- Android 若继续保留 Slim 支持，必须另行完成 Android 构建与启动验证。
