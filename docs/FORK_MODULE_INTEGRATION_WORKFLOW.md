# Fork module integration workflow

This document defines how to re-apply the enabled Fritchii-HDrum modules whenever the upstream Fritchii project publishes a newer version.

The machine-readable module list is `docs/fork-modules.yml`. **That manifest is the source of truth** for which modules are active. Do not rely on chat history or memory alone.

## Current branch pointer (check this first)

- **Current module work branch:** [`feature/v5.8.4-renhdrum-zh-cn`](https://github.com/byronlai99/Fritchii-HDrum/tree/feature/v5.8.4-renhdrum-zh-cn)
- **Purpose:** carries the v5.8.4 port of the enabled module set and the maintenance workflow/manifest.
- **Upstream base:** `Frizti/Fritchii` tag `fritchii-v5.8.4` (`4f5c53149b7e17d1c24445f68d79a83a3b2a17f9`).
- **Stable default branch:** `fix/v5.6-msvc-c4458-packet-shadow`. This is separate and must not be used as the base for v5.8.4 module work.

When continuing this project, read `active_module_branch` in `docs/fork-modules.yml` first. Use that exact branch directly; do not spend time enumerating branches or guessing from branch names. When a new upstream version gets its own integration branch, update this pointer and the manifest's `baseline.working_branch` together.

## Trigger phrase

When the user says **"套入我们的优化模块"**, **"移植我们的 fork 模块"**, or an equivalent instruction, interpret it as:

1. Identify the exact upstream tag/commit the user wants as the new base.
2. Read `docs/fork-modules.yml` from the latest maintained Fritchii-HDrum branch. If the new branch starts from a clean upstream tag, carry this workflow document and manifest into the new working branch as project-maintenance metadata.
3. Apply every module whose `enabled` value is `true`, unless the user names a narrower scope.
4. Do not apply modules marked `false`.
5. Compare each module with the new upstream code before changing anything. If upstream has already incorporated an equivalent fix, verify equivalence and mark it as already upstream instead of duplicating it.

## Current active module set

The following modules are currently enabled in the manifest:

| Module ID | What to re-apply |
| --- | --- |
| `renhdrum` | RenHDrum controller haptics / HD Rumble, SDL3 transport patches, settings UI, runtime DLL |
| `chinese-ui` | Simplified Chinese translation catalog and RenHDrum translations |
| `accurate-vibration-default` | Set `enable_accurate_vibrations` default to `true` |
| `controls-ui-pr36` | Adapt the Controls layout and controller preview changes from upstream PR #36 |

The exact files, source references, build flags, migration notes, and validation requirements live in the YAML manifest. Update that manifest whenever the module scope or source changes.

## Standard procedure for each upstream update

**Baseline gate comes first.** Do not reconcile or port any fork module until the exact upstream tag has passed a Windows x64 MSVC-only baseline build. Preserve the pristine tag, create a separate branch that changes only the workflow matrix to remove non-Windows/MSVC jobs, and run that single supported target. Fix source errors only on a separate MSVC-fix branch. After the clean-source MSVC build, link, and package stages pass, create the module-integration branch and port the enabled modules. This prevents upstream defects from being mixed with RenHDrum/UI integration failures.

### 1. Establish a clean upstream baseline

- Confirm the requested upstream release tag exists and resolve it to its commit SHA.
- Create a dedicated working branch from that exact upstream tag. Do not base it on an unrelated older fork branch unless explicitly requested.
- Record the upstream repository, tag, commit SHA, fork branch, and date in the manifest or update notes.
- Keep the existing stable branch unchanged.

### 2. Reconcile modules against upstream

Process enabled modules one at a time, in manifest order.

For each module:

- Inspect its listed files and all relevant upstream counterparts.
- Check whether the functionality has already landed upstream, changed names, moved files, or been redesigned.
- Port only the missing behavior. Adapt code to the new version; do not blindly cherry-pick old patches or overwrite upstream improvements.
- Keep module changes isolated in focused commits so a failed module can be reverted independently.
- If a module no longer applies or is replaced upstream, explain why and update its manifest status with the user's approval.

### 3. Preserve module-specific requirements

- **RenHDrum:** verify the RenHDrum API/DLL version, SDL3 version compatibility, patch applicability, CMake integration, runtime loading, and supported controller paths.
- **Simplified Chinese:** reconcile the translation catalog, retain new strings, and check that the translation is actually built and selectable.
- **Accurate vibration:** keep `enable_accurate_vibrations` separate from the general vibration and RenHDrum master switches. A changed default must not silently erase an existing user's saved preference.
- **PR #36:** re-check the current Controls page geometry and apply only the still-needed layout fixes. Keep its compact QSS selectors scoped to the Controls widgets.

### 4. Build and package a test artifact

Use the build policy in `docs/fork-modules.yml`. The current policy is Windows x64 MSVC only, no PGO, and test artifact only.

Before packaging:

- Confirm the intended executable exists.
- Confirm the RenHDrum runtime DLL is present and its hash/version is correct.
- Confirm Simplified Chinese translation resources are included.
- Include the immutable `recipes/OoT v131072 (cache v54).nx2recipe` file. The build restores it from the official Fritchii v5.5 Windows package, verifies the source archive SHA-256 (`63112e7125e6174192079475b485f9633509a4a8dfb2630b5f8a96c644abf066`), extracts the single matching recipe entry, and verifies the recipe SHA-256 (`85d944c8557fbeffe02af566c9c4277251ed0ab1a0f2a3ab7a626af6a877c3a1`). Do not regenerate or update this recipe unless the user explicitly requests it.
- Exclude `eden-cli.exe`, `vc_redist*`, and `*.pdb` from the package.
- Upload a short-retention GitHub Actions artifact. Do not create a GitHub Release unless the user explicitly asks.

### 5. Validate and report

Report each module as one of:

- **Applied:** ported and ready for testing.
- **Already upstream:** equivalent behavior is present in the new base.
- **Adapted:** the module needed a version-specific implementation.
- **Blocked:** cannot safely apply or compile; state the exact blocker.
- **Disabled:** excluded by the manifest.

After a successful build, provide the Actions link and artifact link/name, summarize the important changes, and list any controller/UI checks that still need the user's real hardware or visual confirmation.

Do not describe a branch as build-successful until the latest relevant Actions run has completed successfully.

## Adding, disabling, or removing modules

The module list is intentionally editable:

- **Add:** append a new entry to `docs/fork-modules.yml` with a unique `id`, source, files, migration notes, and validation steps. Set `enabled: true`.
- **Temporarily skip:** set `enabled: false`. Keep the entry so its history and reason remain visible.
- **Retire permanently:** remove the entry only after confirming the user no longer wants it and recording the reason in the maintenance notes.
- **Change a module:** update its source/version/files and validation steps together. Do not update only the display name.

A user request to add or remove a module overrides this list for that task, and the manifest should then be updated so the next upstream migration follows the new module set.

## Current baseline note

The manifest currently records upstream `fritchii-v5.8.4` as the baseline. The first 5.8.4 port still requires a fresh successful Actions run and hands-on controller/UI testing; a previous run failed because the port referenced `Settings::nx2_active_mode`, which was absent from that base. That NX2-only reference has been removed from the v5.8.4 RenHDrum path.

## Upgrade failure ledger (carry this forward to every version)

This ledger is part of the upgrade procedure, not a one-time report. When a new upstream version or module is integrated, review every entry below, add any new failure before making the next build attempt, and keep confirmed failures separate from risks that still need validation.

### Workflow portability rule

A module author's GitHub Actions workflow is evidence of how that author builds their own project; it is not automatically compatible with this fork. Do not copy an external workflow wholesale. Extract and reconcile only the prerequisites that the module actually needs, then express them in the fork's own build workflow.

For every integration, compare and record the external workflow against our target build for:

- Runner OS and architecture.
- Compiler/toolchain and warning policy (our release target is Windows x64 MSVC, not an assumed GCC, Clang, MinGW, or PGO build).
- CMake generator, build configuration, options, target names, and dependency versions.
- Qt, SDL3, CPM/fetched source revisions, patches, and any generated resources.
- Output executable name, runtime DLL placement, deployment command, and artifact directory.

A successful configure step, or a green build in the module author's different environment, does not prove that our MSVC targets compile, link, or package correctly. Keep our fork-owned workflow as the authoritative integration test and build the actual deliverable targets.

### Official v5.8.4 workflow reference

For the current baseline, compare against the official [release-binaries.yml](https://github.com/Frizti/Fritchii/blob/fritchii-v5.8.4/.github/workflows/release-binaries.yml), specifically its Windows x64 MSVC job. The fork should retain the upstream runner, MSVC setup, Qt 6.8.3, sccache, Vulkan SDK, Ninja generator, Release configuration, and the yuzu/yuzu-cmd build targets unless a documented module requirement justifies a change.

The current fork intentionally differs in a few places:

- `YUZU_USE_BUNDLED_SDL3=OFF` is required together with `RENHDRUM_SDL_PATCHES=ON`: the fork's `externals/CMakeLists.txt` selects the `sdl3-hdrumble` CPM package and applies the RenHDrum SDL3 patches in this path. Setting it to upstream's `ON` selects `sdl3-ci` and bypasses that custom patch path.
- `ENABLE_RENHDRUM=ON` and `RENHDRUM_SDL_PATCHES=ON` enable the controller integration.
- `ENABLE_QT_TRANSLATION=ON` builds the fork's Simplified Chinese catalog.
- The fork uploads a test artifact only and removes `eden-cli.exe`, `vc_redist*`, and `*.pdb`, following the fork's package policy rather than upstream's release package.
- The test package restores the exact OoT cache-v54 recipe from the official Fritchii v5.5 Windows release using pinned source-archive and recipe hashes, then places it at `build/release/bin/recipes/OoT v131072 (cache v54).nx2recipe` so it is included under `recipes/` in the uploaded artifact.
- Windows deployment uses the upstream command `windeployqt --release --no-translations`. This skips Qt's separately deployed translation catalogs; it does not remove the application's own zh_CN catalog compiled into its Qt resource.

Do not change an intentional module-specific divergence merely to make the YAML look identical to upstream. Record the reason and verify that it selects the required dependency path.
### Confirmed and historical failures

| ID | Evidence / stage | Symptom | Cause / resolution | Prevention |
| --- | --- | --- | --- | --- |
| F-001: MSVC C4457/C4458 name hiding | [Actions run 43043541561](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43043541561), commit a7fe6fdaae4e6c46118454726ba2d4dea69a3c33; Configure passed, Build failed | C4458 for local state hiding a class member in kepler_compute.cpp:1664 and maxwell_3d.cpp:2825; C4457 for local count hiding a function parameter in maxwell_3d.cpp:2377 and :2381 | Ported/local code triggered MSVC name-hiding diagnostics under the fork's build policy. Renamed the affected locals to bypass_state, dispatch_count, unbounded_count, and draw_state. Do not weaken the warning policy to hide these errors. | Before every build, scan all changed and newly added C++ files for C4456/C4457/C4458 and other warnings promoted to errors. Inspect enclosing parameters, class members, and base-class members, not just the exact prior failure lines. |
| F-002: base-version API mismatch | Earlier v5.8.4 RenHDrum integration attempt; recorded in the current baseline note and subsequent source fixes | Code referenced Settings::nx2_active_mode, which is absent from the v5.8.4 base; a RenHDrum NX2-mode function reference was also not part of the selected v0.3.0 integration API | Removed the NX2-only dependency from this v5.8.4 path, including the unused runtime symbol lookup. This is a version compatibility issue, not a reason to add unrelated NX2 settings to the base. | For every imported symbol, verify it exists in the exact target tag's headers and implementation. For dynamic DLL APIs, verify exported symbols against the selected DLL release; never infer API availability from another branch or module author's fork. |
| F-003: external workflow does not match the fork build | Recurring integration risk observed across module ports; individual external workflow files vary by project and version | A module's own CI may use a different OS, compiler, dependency pin, CMake option set, or artifact layout. Its passing workflow may not exercise our Windows x64 MSVC application targets. | Do not transplant external jobs or assume their CI result covers this fork. Convert required setup into our fork-owned workflow and validate the actual yuzu and yuzu-cmd targets. | Complete the workflow comparison checklist above before editing CI. Record the exact runner, compiler, Qt/SDL3 versions, configure options, build targets, and artifact path for each upgrade. |
| F-005: upstream MSVC C4505 flag typo | Actions run 43049341434; compiler command in the failed Build job | Repeated D9014: invalid value `505` for `/w4`; compile command contained `/w4505`. | Confirmed in upstream v5.8.4 `src/CMakeLists.txt`: `/w4505` was intended to disable warning C4505 but is invalid MSVC syntax. Corrected to `/wd4505`, matching the project's existing `/wd4505` usage elsewhere. | When porting MSVC warning policy, verify exact `/wdNNNN`, `/weNNNN`, and `/wN` syntax in the generated compiler command; do not copy warning flags without checking the compiler's interpretation. |
| F-006: NX2 decoder packet name hiding | [Actions run 43049341434](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43049341434), tested commit 2177f5d8e9deed6a1b2976ba428c755010922c18; Build failed | C4458 at `src/video_core/host1x/nx2_video_decoder.cpp:236`: parameter `packet` hid `Nx2VideoDecoder::Impl::packet`. | Renamed only the `SendReceive` parameter to `input_packet` and passed that value to `SendPacket`; the member buffer remains unchanged. | Scan all C++ added or brought forward with the target baseline, not only files touched by the RenHDrum patch. Include nested `Impl` members when checking method parameters for C4458 collisions. |
| F-007: Vulkan precompiled pipeline index shadowing | [Actions run 43058500053](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43058500053), tested commit 996f7f0821c4200ddf6b897364d1fd922af7cc0a; Build failed | C4456 at `src/video_core/renderer_vulkan/vk_pipeline_cache.cpp:5186`: loop variable `index` redeclared while the same function already had `const auto index = KeptIndex()`. | Renamed only the loop variable and its three array accesses to `shader_index`; the kept-cache `index` object and iteration behavior remain unchanged. | Scan the entire containing function for prior declarations before using common loop names such as `index`, `state`, `count`, or `packet`. Treat all C4456/C4457/C4458 errors as the same pre-build static-review family. |
| F-004: package contents drift | Historical packaging iteration reported an archive containing only two files; the exact originating run/log is not retained in this ledger | The produced archive did not contain the expected full application package. | Root cause is not asserted here because the originating run evidence is unavailable. Treat it as a regression symptom, not a diagnosed cause. | Inspect the final archive listing, not just the upload-artifact step. Verify executable, required Qt/runtime files, RenHDrum DLL, translation resources, and recipe/assets; also verify eden-cli.exe, vc_redist*, and *.pdb are absent. |

### Recurring dependency and packaging checks (risk until proven by CI)

- SDL3 patch drift: RenHDrum's custom SDL3 patches are tied to a specific SDL3 source revision. Check patch application against the exact CPM-resolved source and verify the patched declarations/definitions are available to the final link. Do not count a successful CMake configure as proof.
- Runtime API drift: compare required and optional RenHDrum symbols with both the selected header and the actual v0.3.0 DLL exports; check x64 architecture and SHA-256.
- Translation deployment: follow the upstream v5.8.4 Windows command `windeployqt --release --no-translations`; verify the application's own compiled/embedded zh_CN Qt resource and runtime language selection independently from Qt's separately deployed translation files.
- Artifact layout drift: verify the actual output directory and executable name from the target CMake configuration before packaging. Never assume another project's output path or copy step applies here.

For each new failure, append an entry with the run URL/ID, tested commit, failing stage, exact diagnostic or missing file, root cause (or explicitly unknown), fix commit, and a prevention check. Do not erase old entries when a new upstream version is started.

## Pre-build static review gate (must run before Actions)

A successful CMake configure is not proof that the source is MSVC-clean or that the runtime integration is complete. Before requesting a Windows build:

1. **Compile-diagnostic sweep:** inspect every compiler error from the previous run, then statically search all changed files and newly added C++ files for warnings promoted to errors under the target's actual MSVC flags (especially C4456/C4457/C4458 and the project's /we options). Check the full build targets, not only the file that failed last time. Rename shadowing locals instead of weakening warning levels.
2. **CMake/link graph:** verify all RenHDrum targets are added only for supported desktop targets, propagate SDL3 and required include directories, link the runtime driver into the final application path, and make sure the UI source is compiled exactly once. Confirm the SDL3 patch set is applied to the exact resolved SDL3 revision and the patched symbols are available to the application link.
3. **Runtime ABI/package:** compare every required and optional RenHDrum symbol with the selected DLL release/header. Confirm architecture, version and SHA-256, DLL search location, and fallback behavior when the DLL is missing or incomplete.
4. **SDL/controller lifecycle:** inspect hotplug/reconnect, duplicate device registration, joystick/gamepad close order, thread stop/join, audio callback shutdown, HID close, SDL subsystem init/quit reference counting, and lock order. Ensure no RenHDrum worker/callback can use a joystick, stream, or DLL after it is destroyed.
5. **Settings semantics:** check global vs per-game behavior, load/save/cancel/default flows, and old config compatibility. Keep `vibration_enabled`, `enable_accurate_vibrations`, and `hd_rumble_enabled` distinct. A new default must not silently overwrite an existing user's stored choice. Check all new RenHDrum options in the Fritchii per-game tuning/reconcile list when they are intended to be tuned.
6. **Translation pipeline:** confirm `zh_CN.ts` is compiled into the application's Qt resource and loaded at runtime. Separately decide whether Qt's external translation files are needed. `windeployqt --no-translations` skips translations deployed by windeployqt, but does not strip an application translation already embedded in its Qt resource; verify both paths rather than treating the flag as proof that the app's Chinese catalog is missing.
7. **UI and interaction:** check Qt parent ownership, signal/slot connections, accepted/cancelled dialog persistence, slider/combobox index ranges, and that any config UI target is neither omitted nor duplicate-compiled.
8. **Package contract:** check executable name, runtime DLL, Qt runtime files, translations, recipe/config files, and required assets. Confirm `recipes/OoT v131072 (cache v54).nx2recipe` is present with SHA-256 `85d944c8557fbeffe02af566c9c4277251ed0ab1a0f2a3ab7a626af6a877c3a1`. The packaging step must also verify the pinned official v5.5 source archive hash before extraction. Exclude `eden-cli.exe`, `vc_redist*`, and `*.pdb` as defined by the manifest.
9. **Only then run CI:** use one test-artifact build. After CI passes, inspect the artifact contents before handing it to the user. Do not claim hardware behavior is verified without real controller testing.

Record each finding as **fixed before build**, **known non-blocking**, or **requires CI/hardware validation**. Do not knowingly leave a statically identifiable compile or packaging issue for the next Actions run.


### F-008: MSVC C4458 in RefreshTime (service_manager)

- **Workflow run:** [#21](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43065581977)
- **Affected commit:** `4835fe1d2ac861e173f39c367cf9ce0bfc97e965`
- **File / line:** `src/core/core.cpp:253`
- **Diagnostic:** `error C4458: declaration of 'service_manager' hides class member`; the hidden member is `Core::System::Impl::service_manager` at line 656.
- **Cause:** `RefreshTime` declared a local reference with the same identifier as the class member. MSVC /we4458 promoted this shadowing warning to an error.
- **Fix:** Rename only the local reference to `current_service_manager` and update its three `GetService` uses. No service lookup order or runtime behavior changed.
- **Status:** Fixed in commit `9e053b5a7af47e4c8281da9ecf99de60f5f70bb2` and verified by [Run #22](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43070225601). Full Windows x64 MSVC build, package, and artifact upload succeeded; the log contains no compiler errors or D9014.

## Baseline-first upgrade gate (mandatory from the next upstream update)

Before re-applying any fork module, establish that the exact upstream tag can be built by our supported toolchain:

1. Preserve the pristine upstream tag/commit on an untouched reference branch.
2. Create an isolated baseline-test branch from that tag. Before the first run, edit only its GitHub Actions workflow matrix to remove every job except **Windows x64 MSVC**. Do not change C++ source, CMake source, dependencies, or module code in this first test.
3. Run the Windows x64 MSVC workflow and record every compiler error and relevant malformed warning flag. A cancelled non-Windows matrix job is not a build failure; report the Windows MSVC job's own conclusion separately from the overall workflow conclusion.
4. Fix upstream-source build errors on a separate MSVC-fix branch. Keep the pristine tag and the workflow-only baseline test branch unchanged. Re-run until the complete MSVC build, link, and package stages pass.
5. Only then create the module-integration branch and port RenHDrum, Chinese UI, and other enabled modules. Run the fork-owned Windows x64 MSVC workflow again.

Never combine upstream-baseline errors and module-integration errors in one diagnosis. Every ledger entry must say whether it originated in pristine upstream source, the baseline workflow adaptation, or fork-module integration.

## Recent workflow audit (2026-11-11)

The following runs were inspected. Repeated errors are linked to their existing ledger item instead of being counted as new defects:

| Run | Tested source / branch | Result | Confirmed errors / classification |
| --- | --- | --- | --- |
| [#9 / 43048515194](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43048515194) | Integration commit 0f9bf1c4409c92785ba59201ba6bc10db3ecc1dd | Failed | C4458 packet (F-006); C4457 count and C4458 state (same name-hiding family as F-001); repeated D9014 invalid /w4505 (F-005). |
| [#17 / 43049341434](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43049341434) | Integration commit 2177f5d8e9deed6a1b2976ba428c755010922c18 | Failed | C4458 packet in nx2_video_decoder.cpp:236 (F-006); repeated D9014 (F-005). |
| [#18 / 43058482286](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43058482286) | Integration commit 738d32856a | Failed | C4456 index in vk_pipeline_cache.cpp:5186 (F-007); repeated D9014 (F-005). |
| [#19 / 43058492202](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43058492202) | Integration branch | Cancelled | Cancelled run; no additional compiler diagnostic is counted. |
| [#20 / 43058500053](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43058500053) | Integration commit 996f7f0821c4200ddf6b897364d1fd922af7cc0a | Failed | C4456 index in vk_pipeline_cache.cpp:5186 (same F-007 issue as #18). D9014 was no longer present in this log. |
| [#21 / 43065581977](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43065581977) | Integration commit 4835fe1d2ac861e173f39c367cf9ce0bfc97e965 | Failed | C4458 service_manager in core.cpp:253 (F-008). D9014 was absent. |
| [Upstream v5.8.4 / 43068107673](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43068107673) | Pristine official tag 4f5c53149b7e17d1c24445f68d79a83a3b2a17f9 | Windows MSVC job failed; other matrix jobs were cancelled | Four C4459 diagnostics in frizti_settings.cpp (new F-009) and repeated D9014 from the upstream /w4505 typo (F-005). The overall run was cancelled, but its Windows MSVC job itself completed with failure. |
| [#22 / 43070225601](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43070225601) | Integration commit 9e053b5a7af47e4c8281da9ecf99de60f5f70bb2 | Succeeded | Configure, full MSVC Build, Package test binary, and Upload test artifact all succeeded; log contained no compiler errors and no D9014. F-008 verified. |

Run #9 also showed the older F-001 count/state diagnostics again; these are duplicate sightings of that already recorded code family, not a new root cause. Runs #18 and #20 both showed the same F-007 diagnostic before its source rename was carried into a later build. Do not treat cancelled runs as compiler failures.

### F-009: pristine v5.8.4 MSVC C4459 in frizti_settings.cpp

- **Workflow run:** [43068107673](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43068107673); Windows x64 MSVC job 130688435891.
- **Baseline:** pristine official fritchii-v5.8.4 tag, commit 4f5c53149b7e17d1c24445f68d79a83a3b2a17f9; no fork module changes.
- **File / lines:** src/common/frizti_settings.cpp:25, 136, 147, 155.
- **Diagnostics:** four C4459: declaration of 'values' hides global declaration errors. The hidden declaration is Settings::values in settings.h:1075.
- **Cause:** upstream function parameters/local declarations named values shadow the global settings object under the strict /we4459 warning policy.
- **Status:** Open in the pristine upstream baseline. No source fix has been applied to the clean reference branch. Repair on a dedicated MSVC-fix branch by renaming only the conflicting locals/parameters and updating their local uses; preserve /we4459.
- **Related pre-existing defect:** the same job repeated D9014 because upstream v5.8.4 still passes malformed /w4505; this belongs to F-005, not a second new diagnostic family.
- **Prevention:** preflight all baseline C++ sources for C4456/C4457/C4458/C4459 and inspect the generated compiler command for valid MSVC warning switches before the first baseline build.


Run #22 artifact metadata: Fritchii-v5.8.4-RenHDrum-zh-CN-Windows-x64-9e053b5a7af47e4c8281da9ecf99de60f5f70bb2 (SHA-256 8b498bc84acdec0c3c6c80e73ecd570dc9062fc137dc6078dbb9cfa3a34e6042; expires 2026-11-18). The workflow package and upload steps passed; the ZIP's internal file list has not yet been audited separately.
