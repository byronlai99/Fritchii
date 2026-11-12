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
| `renhdrum` | RenHDrum controller haptics / HD Rumble, SDL3 transport patches, settings UI, runtime DLL, OoT gain/low-boost tuning (revision 13) |
| `chinese-ui` | Simplified Chinese catalog, menus/tooltips, Saves and first-run migration translations |
| `fork-branding` | Fritchii-HDrum About/menu identity and matching translations |
| `fork-update-channel` | Own GitHub release feed, exact auto-updater tag/ZIP/digest contract and tests |
| `accurate-vibration-default` | Set `enable_accurate_vibrations` default to `true` without overwriting existing saved choices |
| `controls-ui-pr36` | Adapt the Controls layout and controller preview changes from upstream PR #36 |

The exact files, source references, build flags, migration notes, and validation requirements live in the YAML manifest. Update that manifest whenever the module scope or source changes.

## Standard procedure for each upstream update

**Baseline gate comes first.** Do not reconcile or port any fork module until the exact upstream tag has passed a Windows x64 MSVC-only baseline build. Preserve the pristine tag, create a separate branch that changes only the workflow matrix to remove non-Windows/MSVC jobs, and run that single supported target. Fix source errors only on a separate MSVC-fix branch. After the clean-source MSVC build, link, and package stages pass, create the module-integration branch and port the enabled modules. This prevents upstream defects from being mixed with RenHDrum/UI integration failures.

### 1. Establish a clean upstream baseline

- Confirm the requested upstream release tag exists and resolve it to its commit SHA.
- Create a dedicated working branch from that exact upstream tag. Do not base it on an unrelated older fork branch unless explicitly requested.
- Record the upstream repository, tag, commit SHA, fork branch, and date in the manifest or update notes.
- Keep the existing stable branch unchanged.

### 1A. Read upstream release notes and turn them into a porting checklist (mandatory)

Before touching source code, read the complete release notes for the exact target tag and follow their linked PRs/commits when a note describes behavior that is not self-explanatory. Do not treat the release notes as a substitute for source comparison: notes summarize user-visible changes, while API, build, translation, and edge-case changes may be recorded only in the source diff.

For each release-note bullet, add one row to the upgrade review with:
- the exact upstream note / linked source change;
- affected file, behavior, setting, data path, dependency, or UI;
- classification: **Preserve upstream**, **Re-apply fork module**, **Adapt to new API**, **Already equivalent upstream**, or **Manual validation**;
- the action and acceptance check;
- whether Chinese translation, profile migration, settings persistence, and packaging are affected.

Map release-note topics to the modules and maintenance checks below. One release-note item may touch several areas; do not force each item into only one module.

| Release-note topic | Required review / port action | Existing fork area |
| --- | --- | --- |
| Controller, SDL, vibration, haptics, device discovery, or input settings | Compare SDL3 version and patched source, controller APIs, runtime ABI/DLL exports, input UI, settings persistence, and supported controller paths. Re-apply only the missing RenHDrum integration. | `renhdrum`, `accurate-vibration-default`, controller hardware checks |
| Controls page, controller preview, device selector, mapping layout | Compare the new UI geometry and styling against upstream. Keep only the still-needed compact layout and preview changes; do not duplicate an upstream fix. | `controls-ui-pr36` |
| Settings defaults, per-game tuning, memory profile, or settings reapplication | Check default vs saved values, global vs per-game scopes, tuning revision, and whether the new release intentionally reapplies defaults. Preserve RenHDrum gain/low-boost tuning and user overrides. | `accurate-vibration-default`, RenHDrum tuning, upstream settings |
| DLSS / graphics quality / per-game options | Preserve upstream per-game override and “use global” behavior. Verify config serialization, global fallback, game-specific persistence, and translated labels/help text. Do not re-implement upstream features as a fork module. | Upstream behavior + `chinese-ui` |
| Saves, profile location, migration, import/export, backup, repair | Verify destination profile path, save-copy target, backup-before-write behavior, recovery/fix workflow, and no data loss. Review migration messages and every associated translation. | Upstream profile/storage behavior + `chinese-ui` |
| Language, menus, dialogs, tooltips, first-run prompts | Reconcile all changed/new source strings and exact Qt translation contexts, even if the release notes do not mention localization. | `chinese-ui` |
| Qt, SDL3, Vulkan, compiler, dependencies, CMake, packaging, or updater | Compare upstream workflow and dependency pins; port only the supported Windows x64 MSVC requirements and preserve the fork's runtime DLL, translations, recipe, exclusions, and archive checks. | Build workflow / package contract |

**Example: v5.8.4 release-note review (already incorporated; keep as regression checks on future tags).**

Source: [Fritchii v5.8.4 release notes](https://github.com/Frizti/Fritchii/releases/tag/fritchii-v5.8.4), published 2026-11-11.

| v5.8.4 upstream change | What the next upgrade must preserve / re-check | Classification |
| --- | --- | --- |
| Saves copied through the Saves window now land in Fritchii's own profile; Saves always backs up first, reports the backup path, and offers an Open backup action; the Fix action repairs previously orphaned saves after creating a backup. | Compare the save-copy destination and import/fix code against the new profile root. Check backup is created before copy/import/fix, the path is shown before and after, the backup can be opened, and copied saves appear in the active Fritchii profile. Reconcile all Saves UI strings in the `QObject` context. | Preserve upstream; manual save-data validation |
| Switch 2 games use the 12 GB memory layout while running and restore the previous setting after the game exits; the UI label is “12GB DRAM (Switch 2)” and it is no longer classified as OoT-only. | Preserve temporary runtime switching and restoration. Check setting labels, warning text, OoT/per-game tuning classification, and that the feature is not accidentally forced for Switch 1 games. | Preserve upstream; settings/tuning regression test |
| DLSS quality and preset are available per game under Game Settings > Graphics, with “use global” behavior. | Preserve the per-game override, global fallback, serialization, and UI behavior. Ensure the Chinese catalog includes any changed labels, descriptions, and tooltips. | Preserve upstream; Chinese UI reconciliation |
| Fritchii tuned settings are applied once again with the update, after which users may change them. | Inspect the tuning revision and one-time reapply mechanism. Ensure fork-specific RenHDrum gain/low-boost fields remain in OoT tuning, and do not silently reapply defaults on every launch or erase later user edits. | Adapt upstream tuning revision; re-apply fork-specific tuning |

At the end of each review, maintain a short **release-note → action matrix** in the PR/upgrade notes or manifest. This is the hand-off checklist for the next maintainer; record “no action” explicitly when a note is unrelated. Release-note wording alone is not proof of implementation: verify the actual code and test the behavior.

### 2. Reconcile modules against upstream

Process enabled modules one at a time, in manifest order.

For each module:

- Inspect its listed files and all relevant upstream counterparts.
- Check whether the functionality has already landed upstream, changed names, moved files, or been redesigned.
- Port only the missing behavior. Adapt code to the new version; do not blindly cherry-pick old patches or overwrite upstream improvements.
- Keep module changes isolated in focused commits so a failed module can be reverted independently.
- If a module no longer applies or is replaced upstream, explain why and update its manifest status with the user's approval.

### 3. Preserve module-specific requirements

- **RenHDrum:** verify the RenHDrum API/DLL version, SDL3 version compatibility, patch applicability, CMake integration, runtime loading, and supported controller paths. Also preserve the OoT tuned-settings entries for hd_rumble_gain and hd_rumble_low_boost in src/common/frizti_settings.cpp; src/common/frizti_settings.h must bump Settings::Frizti::Revision when this tuned list/default changes. The current v5.8.4 integration uses revision 13 (upstream v5.8.4 baseline revision 12). Check the one-time reapply behavior and keep later user edits.
- **Simplified Chinese:** reconcile the translation catalog, retain new strings, and check that the translation is actually built and selectable.
- **Accurate vibration:** keep `enable_accurate_vibrations` separate from the general vibration and RenHDrum master switches. A changed default must not silently erase an existing user's saved preference.
- **PR #36:** re-check the current Controls page geometry and apply only the still-needed layout fixes. Keep its compact QSS selectors scoped to the Controls widgets.

### 3A. Reconcile the complete Simplified Chinese UI (mandatory on every upstream update)

Do this for every upstream tag, even when the upstream release notes do not mention translations. A UI string can be added or changed in a C++ file, a Qt .ui form, a dynamically built menu, a tooltip, or a first-run dialog without an obvious translation-related commit.

**Default language behavior (keep this contract):**
- A fresh profile stores `UISettings::values.language` as an empty string. The interface selector's first option is **System**, whose item data is empty.
- `MainWindow::LoadTranslation()` passes `QLocale()` to the translator when that value is empty. Windows' system locale is therefore used on first launch; if no matching catalog can be loaded, the application falls back to English.
- The selector must remain exactly **System / English / 中文**. Do not silently change the fresh-install default to Chinese or English.

**Required translation-port checklist:**

1. Start from the current fork's complete `dist/languages/zh_CN.ts`, not an older upstream copy. Compare the new upstream `dist/languages/zh_CN.ts` and every changed/new `.ui` and source file against it. Merge newly added or changed source strings by context; never replace the fork catalog wholesale or discard existing fork-only entries (RenHDrum, DLSS/FG, Fritchii menus, migration text, and other local UI).
2. Re-scan all changed C++ and Qt UI code for visible text: menu/action labels, submenus, button labels, tab names, tooltips/hover popups, status text, QMessageBox/QDialog/QProgressDialog text, error/recovery messages, update/About dialogs, and strings assembled dynamically. Do not limit the review to `.ui` files or upstream translation diffs. The Fritchii Saves window uses `QObject::tr()`, so every Saves dialog title, button, label, prompt, and result message belongs in the existing `QObject` translation context; its Help-menu action belongs in `FritchiiUi`.
3. Keep translation contexts exact. Typical contexts in this fork include:
   - Qt Designer forms: the class/context generated for the corresponding `.ui` file (for example `MainWindow`).
   - `QObject::tr(...)`: `QObject`.
   - `ConfigureDialog::tr(...)`: `ConfigureDialog`.
   - Fritchii's custom menus: `FritchiiUi`.
   - Left-rail tooltips created by `FritchiiUi::Home`: `FritchiiUi::Home`.
   - Main-window dynamic actions: `MainWindow`.
   If code uses `QCoreApplication::translate("SomeContext", ...)`, the TS context must match that exact string. A translation entry under a similar but different class name does not translate at runtime.
4. Treat first-run profile migration as a permanent translation surface. Check the complete flow: first-run question; source-profile name and last-played label; Saves and keys / Settings / Mods; Copy / Start fresh / Cancel; busy-source and progress messages; successful-copy summary and destination path; linked NAND/SDMC and backup notices; shader-preparation notice; failure, retry, and start-without-migrating choices; and unfinished-import recovery. Keep every dynamic placeholder (`%1`, `%2`, `%3`, `%n`) unchanged in meaning and count.
5. For each added/changed message, provide a complete Simplified Chinese translation. Preserve Qt markup, HTML entities, mnemonic ampersands, line breaks, plural forms, and placeholders. Do not leave an empty translation unless intentionally untranslated and documented.
6. Validate the TS catalog before committing: balanced XML; no nested or duplicate `<context>` blocks; unique context names; no duplicate source keys within a context; valid `<translation>` / `<numerusform>` structure; placeholders match the source. Use Qt's `lupdate` or an equivalent source scan to find newly introduced strings, then run `lrelease` through the actual Windows MSVC build and confirm `zh_CN.qm` is generated successfully.
7. Confirm the catalog is embedded in the application resource under `:/languages/`. CMake's `qt_add_translation()` and `qt_add_resources()` generate and embed the QM catalog. The Windows package intentionally uses `windeployqt --no-translations`; do not re-add an external `translations\\` directory merely to make the application's Chinese UI work.
8. After a successful build, perform a UI spot-check in Chinese for both the classic and Fritchii layouts: Settings > Advanced; all Help/File/Game menu entries; Library/Settings/Help hover tooltips; About; first-run migration; and any newly changed upstream screens. Record anything that still needs real runtime confirmation.

**Do not mark Chinese UI as complete based only on a populated TS file.** The source context must match, `lrelease` must succeed, the catalog must be embedded, and the relevant UI surfaces must be checked.

### 3B. Preserve Fritchii's dedicated profile root and first-run migration

Treat the data-directory migration as a compatibility and data-safety contract, not just a first-launch dialog. Review it on every upstream update that touches paths, saves, keys, settings, imports, or profile management.

**Current path contract (Windows):**
- The normal application profile is `%APPDATA%\\fritchii`.
- A portable `user` directory beside the executable still takes priority when present.
- Legacy profile discovery includes `%APPDATA%\\eden` and `%APPDATA%\\lichii`; the code also recognizes older emulator profile roots (Citron, Sudachi, Suyu, and Yuzu).
- On non-Windows systems, check the current XDG data/config/cache locations and retain portable-mode behavior rather than copying Windows paths literally.

**Migration behavior to preserve:**
1. First launch detects eligible legacy profiles and offers import. Profile selection must use the intended source profile; when both Eden and Lichii exist, confirm the newest-save selection logic still chooses the expected profile.
2. The normal first-run **Copy** path imports settings/configuration, saves and keys, and user-selected mods. Preserve source data. Existing Fritchii data must not be silently merged over or deleted; explicit later imports must retain the established backup behavior.
3. Caches, logs, crash dumps, and screenshots are not copied as part of the profile migration. Shader preparation therefore happens again on the first game launch after migration. Do not copy old shader/cache data merely to avoid this expected one-time work.
4. NAND and SDMC are large. Copy them only when the free-space check allows it; otherwise preserve the link/shared-folder behavior and clearly tell the user to keep the old folder. Verify both the copied and linked cases.
5. Imported configuration may contain absolute paths. Re-map paths inside imported configuration from the legacy root to the new Fritchii root; test paths that are already relative or outside the old profile are left unchanged.
6. A successful migration summary must identify the new destination, say which categories were copied, explain any linked NAND/SDMC, state that the old source folder was not changed on the Copy path, explain the shader preparation, and provide an **Open folder** action.
7. Preserve interrupted-import recovery, migration journal/lock handling, rollback on failure, and backup folders. A retry must not duplicate/partially overwrite data; never delete a source or backup as a shortcut to completing migration.
8. Keep first-run migration text, progress, success, backup, linked-folder, failure, retry, and recovery messages translated. Preserve all Qt placeholders and line breaks.

**Minimum migration regression matrix for the next upstream tag:**
- Fresh install with only an `eden` source profile.
- Fresh install with only a `lichii` source profile.
- Both legacy roots present, with different last-saved times.
- Existing `%APPDATA%\\fritchii` profile already initialized (must not be overwritten by the first-run import path).
- User chooses Start fresh / declines migration.
- Sufficient disk space for NAND/SDMC copy, and insufficient space where the folders must remain linked.
- Interrupted import / pending journal recovery, busy source profile, and retry after a locked file.
- Confirm saves load in-game from the new profile, configuration paths are remapped, the old source remains intact on Copy, and the Open folder action opens the new root.

Do not report profile migration as verified from compilation alone. The migration behavior and real save visibility require runtime testing with safe test profiles; never use the player's only live save folder as a migration test fixture.

### 3C. Preserve fork branding and the built-in update channel

These are fork-owned behaviors, not cosmetic leftovers. Re-apply them when upstream changes the About UI, version metadata, release workflow, updater, download dialog, or package naming.

**Fritchii-HDrum identity:**
- Keep the application Help > About action and About dialog branded **Fritchii-HDrum** in both the classic and Fritchii menu/layout modes. Do not let a rebase restore Lichii/Eden/Fritchii-only labels or make branding conditional on a UI mode.
- Keep the About title/body and matching zh_CN.ts source strings in sync.
- Keep the updater download request User-Agent as Fritchii-HDrum.

**Built-in updater source and exact release contract:**
- src/frontend_common/update_checker.cpp points to the fork's own repository: byronlai99/Fritchii-HDrum. Do not revert it to Frizti/Lichii.
- The built-in updater accepts version tags in the form fritchii-vN, fritchii-vN.M, or fritchii-vN.M.P (legacy frizti-v... tags remain parseable). A suffix such as -preview.34 is not accepted by the current parser.
- For a version N[.M[.P]], the only recognized Windows package is exactly Fritchii-HDrum-vN[.M[.P]]-Windows-x64.zip, attached to the same release/tag at the canonical GitHub download URL. Debug-symbol ZIPs and .7z archives are intentionally not updater packages; .7z may remain as a manual-download alternative.
- GitHub must expose a valid SHA-256 asset digest. Without it, the updater deliberately changes the button to **Open Release Page** rather than installing automatically. Downloads are also checked against the published size/digest before installation.
- A release whose notes contain the exact hidden marker <!-- fritchii:no-update --> is skipped by the updater. Use it for test/joke releases that must remain visible on GitHub but must not be offered in-app.
- The Fritchii update path is Windows-only. The current CheckFritchii() returns no update for non-Windows platforms.

**Important current workflow mismatch (resolve before enabling publish):**

The current .github/workflows/renhdrum-v584-build.yml publish step still generates a tag like v5.8.4-preview.<run number> and an asset named like Fritchii-v5.8.4-RenHDrum-zh-CN-Windows-x64.zip. Neither matches the current updater contract above. Consequently, if that publish step is enabled unchanged, the built-in updater will ignore the release even though the package may download manually. Run #34 left the publish step skipped, so this is a documented pre-publish compatibility issue, not a failed or published release.

Before any future release is published:
1. Choose a canonical version tag accepted by the parser, normally fritchii-v<version>.
2. Name the updater ZIP exactly Fritchii-HDrum-v<version>-Windows-x64.zip. The ZIP must contain the normal complete app package. Keep the optional 7z as a second manual asset.
3. Confirm GitHub exposes the ZIP's sha256: digest in the Release API response.
4. Test the release JSON through SelectFritchiiUpdate() and SelectFritchiiAsset(); verify the running version, next-version ordering, exact repo URL, exact asset name, digest, and hidden-marker behavior.
5. Keep src/tests/common/update_checker.cpp in sync with any change to repository, tag parsing, asset naming, digest handling, or hidden-release behavior.

The current Windows x64 MSVC workflow's Package test binary step packages the executable; it is not evidence that the Catch2 updater unit tests ran. The recent #31–#34 job steps did not show an explicit updater test execution step. Run the updater test target separately or add it explicitly to CI before claiming updater regression tests passed.

### 4. Build and package a test artifact

Use the build policy in `docs/fork-modules.yml`. The current policy is Windows x64 MSVC only, no PGO, and test artifact only.

Before packaging:

- Confirm the intended executable exists.
- Confirm the RenHDrum runtime DLL is present and its hash/version is correct.
- Confirm Simplified Chinese translation resources are included.
- Include the immutable `recipes/OoT v131072 (cache v54).nx2recipe` file. The build restores it from the official Fritchii v5.5 Windows package, verifies the source archive SHA-256 (`63112e7125e6174192079475b485f9633509a4a8dfb2630b5f8a96c644abf066`), extracts the single matching recipe entry, and verifies the recipe SHA-256 (`85d944c8557fbeffe02af566c9c4277251ed0ab1a0f2a3ab7a626af6a877c3a1`). Do not regenerate or update this recipe unless the user explicitly requests it.
- Exclude `eden-cli.exe`, `vc_redist*`, and `*.pdb` from the package.
- Upload a short-retention GitHub Actions artifact. Do not create a GitHub Release unless the user explicitly asks.

**After creating each archive (mandatory final-content audit):**

- Run the archive integrity test on both ZIP and 7z. Integrity success only proves that the container is readable; it does not prove that the package contains the correct files.
- List the members of each finished archive independently (for example, with 7-Zip list output). Confirm the expected package root and required entries: lichii.exe, renhdrum.dll, and recipes/OoT v131072 (cache v54).nx2recipe. Confirm the Qt runtime DLLs/plugins and that the app's zh_CN catalog is embedded in the executable.
- Confirm these are absent from both finished archives: eden-cli.exe, vc_redist*, *.pdb, and an external translations/ directory.
- Record each archive's filename, size, SHA-256, member count, and the result of the required/forbidden-entry checks. Do not use a package-directory listing taken before archive creation as a substitute for inspecting the ZIP and 7z themselves.
- **Current workflow gap:** run #34 performed 7-Zip integrity tests and checked that both archive files existed, but did not enumerate and assert the final archive members. Treat the final-content audit as outstanding until this is added to the workflow or performed explicitly for the uploaded artifact.

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
- Runtime API drift: compare required and optional RenHDrum symbols with both the selected header and the actual v0.3.1 DLL exports; check x64 architecture and pinned SHA-256 `ff397c9e51a976ce8efad625c289589d38e36bf209860d8a87dc91fc17e748f7`.
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

## Recent workflow audit (2026-11-11 to 2026-11-12)

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
| [#23 / 43081539323](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43081539323) | Integration commit 2d72446a8c6c552bd74183d2a01154c2fdfb9d6b | Build succeeded; packaging failed | Pinned recipe lookup expected a full internal ZIP path and found zero entries. Added as F-012. |
| [#25 / 43081887600](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43081887600) | Integration commit 8802040f402756bce225d56b4e7c35ea6beb3fa3 | Succeeded | Recipe lookup by exact basename with an exactly-one-match guard passed; package and artifact upload succeeded. F-012 verified. |
| [#31 / 43145920017](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43145920017) | Integration commit 198712dfb4737287989534496dc880c02964b7fd | Failed | `lrelease` rejected a nested/duplicate `FritchiiUi` context in `zh_CN.ts` (F-010). |
| [#32 / 43149782388](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43149782388) | Integration commit ede4ead9a1de1022ed7eb154455353cbee2f9d23 | Failed | `zh_CN.qm` generated with 2,249 finished entries; compilation then failed on malformed `QObject::tr` string syntax in migration UI (F-011). |
| [#33 / 43152405973](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43152405973) | Integration commit 6fb42027e350e4a242eef3e1712e9f14043840a8 | Succeeded | Full Windows x64 MSVC build, package test binary, and artifact upload succeeded. Verified the F-011 string fix and translation catalog generation. |
| [#34 / 43158492258](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43158492258) | Integration commit 74c2d427e3fbcdd8cb0491da93b1f2342910f1ca | Succeeded | Full build, package test binary, ZIP+7z creation and integrity tests, and artifact upload succeeded. Publish step was skipped; no Release was published. |

Run #9 also showed the older F-001 count/state diagnostics again; these are duplicate sightings of that already recorded code family, not a new root cause. Runs #18 and #20 both showed the same F-007 diagnostic before its source rename was carried into a later build. Run #23 is a package-stage failure, not a compiler failure; run #25 verifies the recipe extraction correction. Runs #31 and #32 exposed independent translation-source failures; runs #33 and #34 verify the fixes and final package pipeline. Do not treat cancelled runs or skipped publishing steps as compiler failures/releases.

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
 
### F-010: malformed nested context in zh_CN.ts blocks lrelease

- **Workflow run:** [#31](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43145920017).
- **Affected commit:** `198712dfb4737287989534496dc880c02964b7fd`.
- **File / line:** `dist/languages/zh_CN.ts:11764`.
- **Diagnostic:** `lrelease error: Unexpected tag <context>`.
- **Cause:** the FritchiiUi translation context was inserted inside another open context and was also emitted twice. Matching opening/closing tag counts alone did not catch this invalid nesting.
- **Fix:** flatten the context tree, keep one top-level `FritchiiUi` context, and check XML nesting plus unique context names before committing.
- **Verification:** Run #32 generated `zh_CN.qm` with 2,249 finished translations and zero unfinished entries. The following Windows x64 MSVC run #33 and run #34 both completed successfully after the catalog structure was flattened.
- **Status:** Fixed and CI-verified; retain the structural TS validation in the mandatory Chinese UI checklist above.

### F-011: MSVC C3688 from malformed QObject::tr expression in migration dialog

- **Workflow run:** [#32](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43149782388).
- **Affected commit:** `ede4ead9a1de1022ed7eb154455353cbee2f9d23`.
- **File / line:** `src/yuzu/user_data_migration.cpp:179`.
- **Diagnostics:** `C3688: invalid literal suffix 'Fritchii'`, followed by parser errors including `C2001: newline in constant`.
- **Cause:** the translation wrapper was accidentally inserted inside a string literal, producing `"QObject::tr("Fritchii ..."`.
- **Translation result:** in the same run, Qt successfully generated `zh_CN.qm` with 2,249 finished translations and zero unfinished entries. The failure happened later while compiling `user_data_migration.cpp`.
- **Fix:** put `QObject::tr(...)` around the two adjacent string literals, not inside a quoted literal:
  `QObject::tr("Fritchii will use its own profile. Your old emulator " "data has not been changed.")`.
- **Status:** Corrected in commit `6fb42027e350e4a242eef3e1712e9f14043840a8` and verified by successful Windows x64 MSVC run [#33](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43152405973) and subsequent run [#34](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43158492258).



### F-012: pinned OoT recipe archive entry path mismatch

- **Failing workflow:** [run #23 / 43081539323](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43081539323), commit `2d72446a8c6c552bd74183d2a01154c2fdfb9d6b`.
- **Stage:** Build completed successfully; Package test binary failed before artifact upload.
- **Diagnostic:** `Expected exactly one 'recipes/OoT v131072 (cache v54).nx2recipe' in the official package; found 0`.
- **Cause:** the script required the archive entry's full path to equal `recipes/OoT v131072 (cache v54).nx2recipe`, but the official ZIP stored the recipe under a different internal path. A stable filename does not guarantee a stable archive directory prefix.
- **Fix:** compare the final path component (`$_.FullName.Split('/')[-1]`) to the exact recipe filename, then require exactly one match. Keep both SHA-256 checks: one for the downloaded official v5.5 archive and one for the extracted recipe. Never choose the first of multiple matches.
- **Verification:** [run #25 / 43081887600](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43081887600) passed Build, Package test binary, and artifact upload after switching to basename matching. The later ZIP+7z package run [#34](https://github.com/byronlai99/Fritchii-HDrum/actions/runs/43158492258) also succeeded.
- **Prevention for future upgrades:** keep the pinned archive and file hashes; inspect ZIP entry names before assuming directory layout; match by exact basename, assert one result, extract that one entry, verify the extracted hash, and finally verify the recipe exists in the output package. A missing or duplicate match must fail packaging before upload.

### F-013: updater release tag and asset naming do not match the publish workflow

- **Finding type:** static integration mismatch; no release was published by the inspected run.
- **Affected files:** src/frontend_common/update_checker.cpp, src/yuzu/updater/update_dialog.cpp, and .github/workflows/renhdrum-v584-build.yml.
- **Updater contract:** repository byronlai99/Fritchii-HDrum; version tags are parsed from fritchii-v... or legacy frizti-v...; the required Windows asset is exactly Fritchii-HDrum-v<version>-Windows-x64.zip.
- **Workflow output currently configured:** tag v5.8.4-preview.<run number>; ZIP Fritchii-v5.8.4-RenHDrum-zh-CN-Windows-x64.zip.
- **Impact:** enabling the current publish step unchanged would produce a GitHub release that the in-app updater cannot parse/select. Manual download remains possible; this is not a compile or archive-integrity failure.
- **Additional requirement:** the release asset needs a valid GitHub SHA-256 digest for automatic installation. With no digest, the updater offers the release page instead. The optional 7z is not selected by the updater.
- **Resolution required before publishing:** align the workflow tag and ZIP name with the exact updater contract, confirm the Release API digest, and run the updater unit tests against the resulting release metadata.
- **Status:** Open; run #34 succeeded but the publish step was skipped. Do not mark auto-update as verified until the release metadata is accepted by the updater.

### Fork release packaging policy

- Every Windows x64 MSVC package build produces both `.zip` and `.7z` archives from the same validated `build/release/bin` directory.
- Run 7-Zip integrity tests against both archives before uploading the workflow artifact.
- Push-triggered builds remain test-artifact only. A manually dispatched build publishes only when `publish_release=true`; the input defaults to `false`.
- The current workflow's preview tag is `v5.8.4-preview.<run number>`; treat it as a manual-download preview only. It is not compatible with the in-app updater contract in F-013. Do not enable publishing until F-013 is closed.
- Attach both the ZIP and 7z archives to the same manually downloadable preview if publication is explicitly approved. For an updater-visible release, use the canonical tag and ZIP filename in §3C; the 7z remains a manual alternative. Preserve the package manifest: include `lichii.exe`, `renhdrum.dll`, the embedded Simplified Chinese catalog and the pinned OoT cache-v54 recipe; exclude `eden-cli.exe`, `vc_redist*` and `*.pdb`.
