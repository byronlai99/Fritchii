# Eden Build Documentation

## Fritchii-HDrum fork maintenance

For recurring upstream upgrades and our optional fork modules, start here:

- **[Current module work branch: feature/v5.8.4-renhdrum-zh-cn](https://github.com/byronlai99/Fritchii-HDrum/tree/feature/v5.8.4-renhdrum-zh-cn)**
- **[Fork module manifest](./fork-modules.yml)**: enabled modules, source references, target files and validation checks.
- **[Fork module integration workflow](./FORK_MODULE_INTEGRATION_WORKFLOW.md)**: the repeatable procedure for each upstream update.

The module work branch is separate from the repository's stable default branch, `fix/v5.6-msvc-c4458-packet-shadow`.

Are you just a casual user? Take a look at our [User Handbook](./user) then!

To contribute to this fork, see [CONTRIBUTING.md](../CONTRIBUTING.md).

This contains documentation created by developers, build instructions, guidelines, instructions/layouts for [cool stuff we made](./CPMUtil.md), and more.

- **[General Build Instructions](./Build.md)**
- **[CMake Options](./Options.md)**
- **[Cross Compiling](./CrossCompile.md)**
- **[Development Guidelines](./Development.md)**
- **[Dependencies](./Deps.md)**
- **[Debug Guidelines](./Debug.md)**
  - **[RenderDoc](./Debug.md#renderdoc)**
- **[CPM - CMake Package Manager](./CPMUtil.md)**
- **[Platform-Specific Caveats](./Caveats.md)**
- **[The NVIDIA SM86 (Maxwell) GPU](./NvidiaGpu.md)**
- **[AMD GPUs and Switch 2 titles](./nx2/amd.md)**
- **[Driver Bugs](./DriverBugs.md)**
- **[Building Older Commits](./build/OlderCommits.md)**
- Subsystems:
  - **[Design Overview](./DesignOverview.md)**
  - **[Dynarmic](./dynarmic/README.md)**
  - **[HOS Kernel](./HosKernel.md)**
  - **[Settings](./Settings.md)**

## Contributing

- **[Coding guidelines](./Coding.md)**
- **[Contributing](../CONTRIBUTING.md)**

## Externals

Other useful resources in general, take a quick read if you need.

- **[SwitchBrew](https://switchbrew.org/wiki/Main_Page)**
- **[IPS file format](https://zerosoft.zophar.net/ips.php)**
- **[IPSwitch file format](https://github.com/3096/ipswitch)**
