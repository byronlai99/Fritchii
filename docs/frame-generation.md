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

The five supplied files were inspected and all are PE32+ AMD64 binaries. They are NVIDIA runtime components, not emulator source files, and are not compiled or copied into the emulator package by this source integration. Place them beside `sl.interposer.dll` in the Streamline plugin directory; do not put them in the game or shader-cache directories. This lets Streamline load the plugins and their dependencies.

## Enabling

Enable the existing **NVIDIA DLSS** setting first, then use the **FRAME GEN** button in the status bar. It cycles through **AUTO**, **2X**, and **OFF**, in that order. The first enable defaults to **AUTO**, which adaptively schedules generated frames toward 60 FPS; **2X** requests one generated frame for each rendered frame.

The status-bar switch can request Frame Generation for the next game start, or change the mode while a compatible game session is running. Frame Generation only operates while the emulator window is focused, as required by NVIDIA.

The optional launch variable is `LICHII_DLSS_FG=1`. Frame Generation remains off by default until enabled from the status bar. If the GPU, Vulkan optical-flow queue, Streamline plugin, or required entry points are unavailable, the emulator keeps the ordinary DLSS path and logs why Frame Generation was skipped.
