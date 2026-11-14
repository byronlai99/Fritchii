# 04 - 构建与验证

本模块记录 Slim 分支专用 Windows CI 调整、当前已知构建问题，以及未来必须通过的验证项目。

## 1. Windows CI 调整

工作流：

- `.github/workflows/renhdrum-v584-build.yml`

已实施改动：

- 将 `Fritchii-hdr-slim` 加入该 Windows MSVC 测试构建的触发分支。
- 对 Slim 使用独立的 build tag：`fritchii-hdr-slim`。
- 对 Slim 使用独立的测试压缩包命名，避免和原 feature 分支构建产物混淆。
- 原 `feature/v5.8.4-renhdrum-zh-cn` 的构建标签和产物命名保留。

当前工作流执行的是 Windows x64 MSVC 构建，不代表 Android 已经构建或测试。

## 2. 已发现的资源错误

Qt 资源清单：

- `src/yuzu/fritchii/fritchii.qrc`

清单引用：

- `whats-new.md`

Slim 初期分支缺少：

- `src/yuzu/fritchii/whats-new.md`

因此 Ninja 在生成 / 构建 `qrc_fritchii.cpp` 时失败，典型错误是：

```text
ninja: error: '.../src/yuzu/fritchii/whats-new.md',
needed by 'src/yuzu/yuzu_autogen/.../qrc_fritchii.cpp',
missing and no known rule to make it
```

修复方式：新增 `src/yuzu/fritchii/whats-new.md`，并保留 `fritchii.qrc` 中的有效引用。

这不是 C++ 源代码编译错误，而是 Qt resource manifest 引用的文件缺失。以后新增、移动或删除 Qt 资源时，应同时检查 `.qrc` 文件。

## 3. 构建记录快照

以下是整理记录时已确认的状态：

| Run | 对应提交 | 结果 | 说明 |
|---|---|---|---|
| #41 | `44f98476ab11b9e8923ee80a650b9784ea0ea68b` | Failed | CMake 配置通过；Build 阶段因 `whats-new.md` 缺失而失败 |
| #42 | `d6cbe5e7b1e391b86e1e2cfd09ae3dbe8d0ca3c7` | Cancelled | 新提交触发后被后续 run 取消 |
| #43 | `a1fa4f0b1988aee5650c052411a840264b01f2e9` | 记录时仍在运行 | 已包含 `whats-new.md`；尚未得到完整 C++ 编译结论 |

查看最新结果：

https://github.com/byronlai99/Fritchii-HDrum/actions/workflows/renhdrum-v584-build.yml?query=branch%3AFritchii-hdr-slim

**重要：目前不能记录为“Slim 已成功编译”。** #41 的失败原因已定位，缺少的资源文件已补入，但需要后续成功的 Windows MSVC Build run 才能确认代码通过编译。

## 4. 上游升级后的静态扫描

在源代码根目录运行。以下扫描是定位线索，不是自动删除清单：

```bash
# 外部密钥管理和导入入口
rg -n "KeyManager|KeyFileExists|BaseDeriveNecessary|InstallKeys|areKeysPresent|reloadKeys|prod\\.keys|title\\.keys|KeysDir|use_dev_keys" src

# Ticket / NAX / 旧错误状态
rg -n "SetTicketKeys|Ticket::Read|NAX|ErrorMissingProductionKeyFile|ErrorMissingHeaderKey|ErrorMissingTitlekey|ErrorMissingKeyAreaKey|ErrorBadNAX" src

# 已删除文件是否还被 CMake / 其他源文件引用
rg -n "key_manager\\.(cpp|h)|partition_data_manager|xts_encryption_layer|xts_archive|loader/nax" src

# Qt 资源清单引用
rg -n "whats-new\\.md" src/yuzu/fritchii

# 检查补丁空白和冲突标记
git diff --check
rg -n "^(<<<<<<<|=======|>>>>>>>)" src
```

注意事项：

- 不要把 `rg` 命中的所有 `AES`、`crypto`、`OpenSSL` 内容删除。
- 在确认某个函数只服务于外部主机密钥路径后，才能删除它。
- 扫描到 `KeyManager` 字样时，先区分文档、注释、测试和真实代码依赖。
- 删除源文件后，要同步更新 CMake、资源清单、JNI 注册、翻译资源和相关错误映射。

## 5. 最低测试矩阵

### 构建

- [ ] CMake configure 成功。
- [ ] Windows x64 MSVC 完整 Build 成功。
- [ ] ZIP / 7z 测试包生成和校验成功。
- [ ] Android Gradle / NDK 构建成功（若继续保留 Android Slim）。

### 内容加载

- [ ] 已解密 NUP 可以被识别并启动。
- [ ] 已解密游戏目录可以被识别并启动。
- [ ] 加密 NCA 被明确拒绝，错误信息正确。
- [ ] 加密内容不会因缺失 keys 而触发旧式安装提示。
- [ ] NSP / XCI 的当前实际支持边界经过测试，并与文档一致。
- [ ] SD 注册内容不会再尝试 NAX 解密。

### 游戏运行与外围功能

- [ ] OoT 启动、场景切换和着色器编译。
- [ ] ExeFS / RomFS 正常读取。
- [ ] 存档读写、备份和重启后恢复。
- [ ] 控制器识别、输入和震动。
- [ ] RenHDrum runtime 能加载；不可用时的回退行为正常。
- [ ] Firmware Manager 的固件检查行为正常。

## 6. 发布门槛

在构建成功和最小回归完成前，Slim 只应作为实验分支使用。CI 的“配置成功”、单个资源步骤成功、或 UI 不再显示密钥安装菜单，都不能替代完整编译和运行时测试。
