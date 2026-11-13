# Contributing to Fritchii-HDrum

Thank you for your interest in Fritchii-HDrum. This repository is an independently maintained Windows x64 fork of [Fritchii](https://github.com/Frizti/Fritchii), with [RenHDrum](https://github.com/trzz1214/RenHDrum) integration and fork-specific fixes, translations, build automation and maintenance work.

Fritchii-HDrum is experimental software. Contributions should preserve upstream work where possible, keep fork-specific changes isolated and maintainable, and clearly describe what has and has not been tested.

## Where to contribute

- **Bugs and feature requests:** Open an issue in [this repository](https://github.com/byronlai99/Fritchii-HDrum/issues).
- **Code and documentation changes:** Open a pull request against the active development branch shown in the repository. For the current v5.8.4 line, that branch is `feature/v5.8.4-renhdrum-zh-cn`.
- **Upstream Fritchii issues:** If a problem also exists in an unmodified upstream Fritchii build, check [Frizti/Fritchii](https://github.com/Frizti/Fritchii) and report it upstream when appropriate.
- **RenHDrum issues:** For defects in RenHDrum itself, check [trzz1214/RenHDrum](https://github.com/trzz1214/RenHDrum). Please include the RenHDrum version and explain whether the issue reproduces without the Fritchii-HDrum integration.

Please do not assume that an upstream issue is automatically a Fritchii-HDrum issue, or that a fork-specific change should be sent upstream unchanged.

## Before making changes

1. Check the current branch, recent commits and the upstream Fritchii version/tag used as the baseline.
2. Read the relevant upstream release notes and compare them with the current fork before porting changes.
3. Check the fork's module manifest and integration notes before modifying an existing fork-specific module.
4. Keep changes focused. Avoid unrelated refactoring, broad formatting changes or replacing working fork logic without a specific reason.
5. Do not commit generated build output, local user data, game files, keys, firmware, personal logs or proprietary runtime files.

When updating to a new upstream version, preserve upstream changes first, then reapply or adapt the fork modules. Do not blindly copy old fork files over newer upstream files.

## Build and validation expectations

The maintained release target is **Windows x64 with MSVC**. Other platforms or build configurations may be useful for development, but they are not release validation unless the maintainers explicitly add them to the supported build matrix.

Before submitting a code change:

- Follow the upstream [build guide](docs/Build.md) and [coding guidelines](docs/Coding.md).
- Use a clean and reproducible Windows MSVC build configuration.
- Review the GitHub Actions run and its logs. A successful workflow is evidence only for the steps that actually ran.
- Run relevant unit tests where available, especially tests affected by the change.
- For controller changes, test the applicable controller and connection type where practical.
- For translation changes, compile the translation catalogs and inspect the affected UI strings in the application.
- For packaging changes, inspect the actual ZIP and 7z contents, not only the archive creation exit code.
- Clearly state any checks that could not be performed.

A successful compile does **not** prove gameplay compatibility, correct rendering, stable performance or real-controller haptics. Do not describe a feature as fully tested unless it was tested in the corresponding runtime scenario.

## Fork-specific contribution areas

### RenHDrum and controller haptics

Changes to RenHDrum integration should:

- State the RenHDrum version and, when applicable, the exact upstream asset or commit used.
- Keep third-party binaries pinned and verify their SHA-256 hash when they are downloaded during CI.
- Preserve a safe fallback to standard rumble when an enhanced haptics path cannot be initialized.
- Avoid claiming support for every controller or connection mode based only on device detection.
- Document controller model, USB/Bluetooth connection and runtime prerequisites for any new capability.


### Chinese translation and localization

Translation contributions should:

- Preserve the existing English and Chinese language choices and the application's system-language behavior.
- Keep translation contexts and source strings valid for Qt Linguist tools.
- Update the appropriate `.ts` files and compile them to `.qm` as part of the build.
- Compare changed upstream UI strings against the Chinese translation so new dialogs, settings and explanations are not silently left untranslated.
- Avoid changing unrelated translations or introducing duplicate/nested translation contexts.

Please include screenshots or a concise list of the UI locations checked when practical.

### Modular fork maintenance

Fork-specific features should remain identifiable and, where practical, isolated in their own modules or clearly scoped patches.

When adding or changing a module:

- Update the fork module manifest and relevant integration documentation.
- Record the upstream baseline, source reference, affected files and any required migration or reapplication steps.
- Document dependencies, build integration, configuration keys and fallback behavior.
- Identify conflicts likely to occur during the next upstream update.
- Do not silently overwrite upstream files when a small targeted patch will work.

### Release packaging

Release packaging changes must preserve the required application runtime and remove development-only files.

For the Windows release package:

- Include the intended emulator executable, required runtime DLLs, translations and the required recipe assets.
- Keep the release archive names consistent with the updater's expected naming convention: `Fritchii-HDrum-v<version>-Windows-x64.zip` and, when produced, the matching `.7z` archive.
- Do not include `eden-cli.exe`, `vc_redist*`, `*.pdb` or other explicitly excluded development artifacts.
- Do not add a separate external translations folder when translations are embedded or packaged in the expected location.
- List and inspect the final ZIP and 7z members, verify required and forbidden files, and record archive hashes.
- Keep publishing disabled during ordinary validation builds. A successful CI artifact is not the same as a published release.

Updater changes must be checked against the actual release asset name, release tag format and published asset digest. Test the updater's version/tag parsing and asset selection before enabling release publication.

## Pull request checklist

Please include the following in your pull request description:

- [ ] The target branch and upstream baseline are identified.
- [ ] The reason for the change and the affected components are explained.
- [ ] Fork module documentation or manifest is updated when applicable.
- [ ] Windows x64 MSVC build results are reported.
- [ ] Relevant unit tests and workflow results are reported.
- [ ] Runtime, controller or UI validation is described separately from compile checks.
- [ ] ZIP/7z contents and hashes are checked when packaging is changed.
- [ ] No game files, keys, firmware, personal data, secrets or unrelated generated files are included.

For a bug report, include the exact Fritchii-HDrum build, game version where relevant, Windows version, CPU, GPU and driver version, reproduction steps, controller model and connection type where relevant, and a log from the affected session. Review logs for personal information before attaching them.

## Licensing and third-party work

By contributing, you agree that your contributions are provided under the repository's applicable license. Retain original copyright notices and third-party license notices when modifying inherited code.

Fritchii-HDrum incorporates work from upstream emulator projects and third-party components. Do not submit code, binaries or assets unless you have the right to distribute them. See [LICENSE.txt](LICENSE.txt) and the relevant third-party license files for details.

Thank you for helping keep Fritchii-HDrum maintainable, testable and clear about its capabilities.
