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
