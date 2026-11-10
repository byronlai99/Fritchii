# DLSS Frame Generation

Fritchii-HDrum's Frame Generation integration is an opt-in extension of the existing NVIDIA Streamline DLSS path.

## Requirements

- Windows and an NVIDIA RTX 40-series or newer GPU.
- A current NVIDIA graphics driver.
- NVIDIA Streamline 2.14.16 or compatible, with the DLSS-G, Reflex and PCL plugins installed.
- The following x64 runtime files in the same Streamline plugin directory as `sl.interposer.dll`:
  - `sl.dlss_g.dll`
  - `nvngx_dlssg.dll`
  - `sl.reflex.dll`
  - `sl.pcl.dll`
  - `NvLowLatencyVk.dll`

The five supplied files were inspected and all are PE32+ AMD64 binaries. They are NVIDIA runtime components, not emulator source files; do not place them in the game or shader-cache directories. Keep them beside `sl.interposer.dll` so Streamline can load the plugins and their dependencies.

## Enabling

Enable the existing **NVIDIA DLSS** setting first, then enable **FRAME GEN 2X** in the status bar. The status-bar switch can request Frame Generation for the next game start, or toggle it while a compatible game session is running. Frame Generation only operates while the emulator window is focused, as required by NVIDIA.

The optional launch variable is `LICHII_DLSS_FG=1`. The default remains off. If the GPU, Vulkan optical-flow queue, Streamline plugin, or required entry points are unavailable, the emulator keeps the ordinary DLSS path and logs why Frame Generation was skipped.
