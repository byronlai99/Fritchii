# 03 - 桌面与 Android 前端

本模块移除面向用户的密钥安装体验和 frontend 对 KeyManager 的直接调用。核心规则是：**前端不再提供外部主机密钥导入，也不再因缺少密钥而阻止已解密内容启动。**

## 1. Common / Frontend Common

改动文件：

- `src/common/fs/fs_paths.h`
- `src/common/fs/path_util.cpp`
- `src/common/fs/path_util.h`
- `src/common/settings.h`
- `src/frontend_common/content_manager.h`
- `src/frontend_common/firmware_manager.cpp`
- `src/frontend_common/firmware_manager.h`

已实施事项：

- 移除专用 keys directory path 定义和初始化。
- 移除 `use_dev_keys` 设置项及其用途。
- 移除 `AreKeysPresent()` 等依赖 KeyManager 的密钥存在检查。
- 移除 frontend common 的 key-install 入口。
- 从 Firmware Manager 移除 InstallKeys / KeyInstallResult 等安装密钥流程及其密钥前置条件。

注意：Firmware Manager 本身没有整体删除。固件存在性、版本或完整性检查仍然存在；不要把“删除外部密钥安装前置条件”误写成“模拟器不再需要固件”。

## 2. Desktop Qt

改动文件：

- `src/qt_common/qt_string_lookup.h`
- `src/qt_common/util/content.cpp`
- `src/qt_common/util/content.h`
- `src/yuzu/main.ui`
- `src/yuzu/main_window.cpp`
- `src/yuzu/main_window.h`

已实施事项：

- 删除 Tools 菜单的 Install Decryption Keys 操作。
- 删除对应 QAction 和 MainWindow slot。
- 删除启动时的密钥存在检查和缺失密钥弹窗。
- 删除开发密钥 reload 路径。
- 删除 Qt Content utility 中的密钥导入操作。
- 启动初始化改为设置固件版本，不再因缺少外部密钥阻断已解密内容启动。
- 清理已废弃的密钥安装结果文字和错误提示。

上游升级时要检查：菜单动作可能已迁移到新的 Settings / Tools 页面，启动检查也可能由 MainWindow 搬到 GameList、ContentManager 或后台 ViewModel。应以行为和调用链为准，不能只按旧文件名找删除点。

## 3. Android / JNI

改动文件：

- `src/android/app/src/main/java/org/yuzu/yuzu_emu/NativeLibrary.kt`
- `src/android/app/src/main/java/org/yuzu/yuzu_emu/fragments/InstallableFragment.kt`
- `src/android/app/src/main/java/org/yuzu/yuzu_emu/fragments/SetupFragment.kt`
- `src/android/app/src/main/java/org/yuzu/yuzu_emu/model/HomeViewModel.kt`
- `src/android/app/src/main/java/org/yuzu/yuzu_emu/ui/main/MainActivity.kt`
- `src/android/app/src/main/java/org/yuzu/yuzu_emu/utils/InstallableActions.kt`
- `src/android/app/src/main/jni/native.cpp`
- `src/android/app/src/main/res/drawable/ic_key.xml`

已实施事项：

- 移除 prod.keys 安装向导、选择文件入口和缺少 keys 的提示。
- 移除 Amiibo keys 导入 UI / handlers。
- 移除 Kotlin 到 Native 的 `installKeys`、`areKeysPresent`、`reloadKeys` API。
- 移除 JNI 中对应导出函数和 C++ key install 实现。
- 移除 HomeViewModel 的 checkKeys 状态及相关触发流程。
- 删除只用于 keys 安装流程的 key 图标资源。

上游升级时需要同时检查 Kotlin external declarations 与 JNI 实现。只删除其中一侧会造成 Native 方法链接或调用不一致。

## 4. Android 翻译资源

已清理多个 locale 中过时的密钥安装文字，并调整简体中文、繁体中文和通用提示，使其反映已解密内容的使用方式。

涉及目录包括：

- `src/android/app/src/main/res/values*/strings.xml`
- `src/android/app/src/main/res/values/strings.xml`

这里有多批分阶段提交，不是一个文件的单次清理。上游新版本的翻译资源通常会新增或改名字符串，移植时应：

1. 先删除 UI / Kotlin 引用；
2. 再用资源名称搜索所有 locale；
3. 删除确定不再引用的 keys 文案；
4. 更新保留的说明文案；
5. 检查 XML 是否可解析，以及资源引用是否完整。

不要只清除简体中文而留下其他语言中的旧安装流程，也不要删除仍被其他设置页面使用的通用 key / Amiibo 资源。

## 5. 前端验收清单

- [ ] Desktop Tools / Settings 中没有密钥导入动作。
- [ ] 启动不再显示缺少 prod.keys 的提示。
- [ ] 不再创建或要求 keys 目录。
- [ ] Firmware Manager 仍能进行固件存在性和版本检查。
- [ ] Android Setup 流程不再要求 prod.keys。
- [ ] Android Installable 页面不再展示 console key / Amiibo key 安装入口。
- [ ] Kotlin external methods 与 JNI 导出完全同步。
- [ ] 所有 locale 不再出现无用的 keys 安装文案。
- [ ] Android 编译和启动验证完成（当前尚未验证）。
