# 05 - 上游升级移植流程

本手册用于未来 Frizti/Fritchii 出现新版本时，将本项目的 Slim 行为重新移植到新上游。目标是复用本次的设计和模块边界，避免每次都从零重新分析。

## 1. 基本策略

**不要把旧 Slim 分支整体直接覆盖到新上游，也不要一开始就批量删除同名文件。**

新上游可能已经：

- 重命名 KeyManager 或把功能拆到多个 service；
- 改变 NCA / NSP / XCI / NAX loader 的调用关系；
- 将桌面密钥 UI 移到新的 ViewModel 或 Settings 页面；
- 改动 Android JNI 注册方式；
- 修改 AES key 类型、Fssystem callback 或 CMake target；
- 自行移除某些旧逻辑，使旧补丁不再需要。

应移植的是“行为和边界”，不是旧文件的机械差异。

## 2. 开始前的准备

1. 选择官方新版本 tag 或明确的上游分支，记录 commit SHA。
2. 在私有仓库建立新的移植分支，不要直接改原 feature 分支。
3. 保留当前 `Fritchii-hdr-slim`，不要重写这次 22 个实施提交。
4. 对新上游与当前 feature baseline 做比较，记录哪些文件已改名、删除或重构。
5. 先尝试只合入上游，不混入 Slim 补丁，确保新上游本身能构建。

建议命名示例：

```text
upstream-vX.Y.Z
slim-vX.Y.Z-port
```

## 3. 对照本次变更

本次代码移植范围：

- 原始基础：`9dcd5e280f272671b076ecad476da1e2216954ff`
- Slim 代码快照：`a1fa4f0b1988aee5650c052411a840264b01f2e9`
- 本次 Slim 实施提交数：22

先检查文件级变化：

```bash
git diff --stat 9dcd5e280f272671b076ecad476da1e2216954ff..a1fa4f0b1988aee5650c052411a840264b01f2e9

git diff --name-status 9dcd5e280f272671b076ecad476da1e2216954ff..a1fa4f0b1988aee5650c052411a840264b01f2e9
```

具体文件和提交可查阅 [06 - 文件清单与提交索引](06-file-map-and-commits.md)。

## 4. 移植顺序

### 模块 A：核心密钥依赖

参考 [01 - 核心密钥与密码层](01-core-crypto.md)。

- 找到新版本对应的密钥管理、派生和 reload 逻辑。
- 将通用 AES Key 类型从密钥管理器解耦。
- 将 Fssystem 加密 key callback 改为明确拒绝外部密钥派生。
- 删除 Ticket / Title Key 依赖。
- 只在调用者清空后删除专用文件，并同步更新 CMake。

先编译 core 相关 target，避免后续 UI 改动叠加 C++ 基础错误。

### 模块 B：内容加载边界

参考 [02 - 内容加载与解密边界](02-content-loading.md)。

- 为所有 NCA 打开入口建立 decrypted-only gate。
- 取消 NAX / SD XTS archive 解密路径。
- 移除 NSP Ticket 和 Title Key 设置流程。
- 清理旧的 key/NAX 错误状态。
- 再决定是否进一步移除 NSP / XCI 容器 loader。此项是独立决策，不能误认为本次已经做完。

### 模块 C：桌面前端

参考 [03 - 桌面与 Android 前端](03-frontends.md)。

- 移除密钥安装菜单和对话框。
- 删除 startup key check。
- 清理 keys directory / use_dev_keys 设置。
- 删除 Qt Content utility 的安装密钥入口。
- 保留 Firmware Manager 的固件检查能力。

### 模块 D：Android / JNI / 翻译

- 移除 SetupFragment 的 prod.keys setup。
- 移除 InstallableFragment 的 console / Amiibo key 安装卡片。
- 同步删除 Kotlin external declarations 与 JNI 函数。
- 删除 HomeViewModel 密钥检查状态及调用。
- 检查所有 locale 的 strings.xml，最后才删除失效字符串。
- 单独执行 Android 构建；Windows CI 不覆盖此模块。

### 模块 E：构建配置和资源

- 检查 `src/core/CMakeLists.txt` 中是否仍引用删除文件。
- 检查 qrc / Qt resource 引用的文件是否实际存在。
- 检查 workflow 是否包含新的分支名和独立构建 tag。
- 确认 Slim 测试包命名不会覆盖普通 feature 分支产物。

## 5. Cherry-pick 还是重新应用？

由 Git 历史决定，不要预设。

### 如果新上游包含共同祖先

可以尝试按模块 cherry-pick 旧实施提交。建议每次只移植一个逻辑模块，遇到冲突时人工核对新上游行为，不能直接选“全部接受 ours”。

### 如果新上游历史不包含共同祖先

不要强行 cherry-pick。以本目录的模块说明和文件地图为规格，在新代码结构中重新实现同样的行为。旧补丁可用于理解具体改动，但不应成为必须照搬的 diff。

### 需要特别小心的冲突

- AES 类型已在新版本移动到其他公共头文件；
- Fssystem crypto callback 的参数或枚举改变；
- NCA decrypted 状态识别方式改变；
- 新增 Loader / ContentManager 入口绕过旧 gate；
- Android JNI 从静态 external method 改为注册表；
- Qt key check 被移动到后台启动流程；
- 新版多了密钥导入按钮、命令行参数或设置项；
- CMake 改为 object library / target source grouping。

## 6. 每次移植的提交切分

建议以如下粒度提交：

1. `slim(core): decouple generic AES types from key manager`
2. `slim(core): reject encrypted NCA without external keys`
3. `slim(loader): remove legacy NAX and ticket key paths`
4. `slim(frontend): remove desktop key import and startup checks`
5. `slim(android): remove key setup UI and JNI APIs`
6. `slim(android): clean obsolete key strings`
7. `slim(build): update CMake resources and Slim CI`
8. `slim(docs): record upstream port and validation results`

实际顺序可以因上游依赖关系调整，但每个提交应有单一意图，方便回滚、比较和解决冲突。

## 7. 上游新版本移植完成标准

- [ ] 已记录上游 tag、commit SHA 和新移植分支。
- [ ] 已对照旧文件地图，记录新增、重命名和删除文件。
- [ ] 不再有外部密钥安装、扫描、reload 或启动阻断流程。
- [ ] 通用 AES 和游戏运行必需的内部密码功能仍保留。
- [ ] 所有 NCA 加载入口都遵守 decrypted-only 规则。
- [ ] NAX / Ticket 旧路径不会被其他 loader 重新引入。
- [ ] Desktop、Android、JNI、翻译引用完整。
- [ ] CMake 和 Qt resource 清单完整。
- [ ] Windows x64 MSVC 构建成功。
- [ ] Android 构建成功（若继续支持）。
- [ ] OoT、NUP、游戏目录、ExeFS、RomFS、存档和控制器回归完成。
- [ ] 文档更新了新的 baseline SHA、移植提交和已知限制。

## 8. 防止误操作

- 不要修改原始 feature 分支来做 Slim 实验。
- 不要把整个 `src/core/crypto` 当成可删除目录。
- 不要只删除 UI 而留下启动时的 key check。
- 不要只删除 Kotlin 方法而遗漏 JNI，或反过来。
- 不要只更新简体中文翻译，遗漏其他 locale。
- 不要删除一个文件后忘记同步 CMake / qrc。
- 不要把“CMake configure 成功”记录为“编译成功”。
- 不要把“加密 NCA 被拒绝”扩大描述为“所有 NSP / XCI 代码都已删除”。
