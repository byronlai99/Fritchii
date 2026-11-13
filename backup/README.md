# Fritchii-HDrum document cleanup archive manifest (historical)

Snapshot branch: `feature/v5.8.4-renhdrum-zh-cn`
Snapshot commit: `d4fa6f74a20fc1e4737cb708ff950ca58e0504fe`
Created: 2026-11-14

**Archive status:** The ZIP was deleted on 2026-11-14 at the user's request. This manifest is retained as a historical record of the cleanup scope; the archived file contents are no longer stored in this repository.

At the time the archive was created, the original source files were still present at their original paths.

## 1. Whole-file removal candidates (21 files)
These documents are entirely about specific games or title-specific workarounds. Review the archive contents before approving deletion.

- `docs/OOT_VISUAL_WORKAROUND.md`
- `docs/STARFOX_AFTERMATH_DIAGNOSTIC.md`
- `docs/STARFOX_COMMUNITY_CRASHES.md`
- `docs/STARFOX_UPSCALE_STANDIN.md`
- `docs/FE_UPSCALE_STANDIN.md`
- `docs/kirby-white-world.md`
- `docs/bananza-atomic-fix.md`
- `docs/bananza-compute-followup.md`
- `docs/bananza-diagnostics.md`
- `docs/bananza-global-atomics.md`
- `docs/bananza-live-trace-followup.md`
- `docs/bananza-pacing-followup.md`
- `docs/bananza-performance-candidates.md`
- `docs/bananza-profile-fixes.md`
- `docs/bananza-query-trace.md`
- `docs/bananza-timing-trace.md`
- `docs/bananza-upstream-comparison.md`
- `docs/duskbloods-bindless-surfaces.md`
- `docs/duskbloods-uniform-immediate-shift.md`
- `docs/nx2/oot-network-bypass.md`
- `docs/performance/MKW-INTEGRATION.md`

## 2. Review-only console ecosystem guides (3 files)
These mention platform-specific ecosystem features. They may contain reusable material, so they are not classified as automatic whole-file deletions.

- `docs/user/UsingAmiibo.md`
- `docs/user/InstallingAtmosphereMods.md`
- `docs/user/CFW.md`

## 3. Shared documents to edit in place (5 snapshots included for reference)
These contain both reusable project information and references that should be sanitized rather than deleting the whole file.

- `src/yuzu/fritchii/whats-new.md`
- `README.md`
- `AGENTS.md`
- `CONTRIBUTING.md`
- `docs/user/Controllers.md`

## 4. Other shared docs found in the audit
Examples include `docs/native-dlss-replacement.md`, `docs/nx2/amd.md`, `docs/nx2/microphone.md`, the shader-sweep reports, video-playback audit files, and several user guides (Mods, Multiplayer, SteamROM, GyroControls, DLSS, UsingCheats, Testing, CFW/Atmosphere references). These were not put in the whole-file deletion set because they contain reusable technical or user guidance.

## 5. Historical GitHub Releases
The 12 release descriptions were checked separately. Descriptions with relevant references include `Fritchii-HDrum v5.8.4`, `Fritchii v5.6 build 63`, and `Fritchii v4.11 build 27`. Release text should be edited in place after review; it is not a source file and is not included as a delete candidate here.

The deleted ZIP contained the original file contents with their repository-relative paths grouped into `originals/`, `review-only/`, and `edit-in-place/`.
