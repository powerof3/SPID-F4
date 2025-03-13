# Spell Perk Item Distributor F4

A F4SE plugin that can be used to add spells/perks/items and more, to every NPC in the game, using config files.

- [Nexus](https://www.nexusmods.com/fallout4/mods/48365)
- [VR Version](https://www.nexusmods.com/fallout4/mods/85756)

## Requirements

- [CMake](https://cmake.org/)
  - Add this to your `PATH`
- [PowerShell](https://github.com/PowerShell/PowerShell/releases/latest)
- [Vcpkg](https://github.com/microsoft/vcpkg)
  - Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
- [Visual Studio Community 2019](https://visualstudio.microsoft.com/)
  - Desktop development with C++
- [CommonLibSSE](https://github.com/powerof3/CommonLibSSE/tree/dev)
  - You need to build from the powerof3/dev branch
  - Add this as as an environment variable `CommonLibSSEPath`
- [CommonLibVR](https://github.com/alandtse/CommonLibVR/tree/vr)
  - Add this as as an environment variable `CommonLibVRPath`

## User Requirements

- [Address Library for SKSE](https://www.nexusmods.com/skyrimspecialedition/mods/32444)
  - Needed for SSE
- [VR Address Library for SKSEVR](https://www.nexusmods.com/skyrimspecialedition/mods/58101)
  - Needed for VR

## Register Visual Studio as a Generator

- Open `x64 Native Tools Command Prompt`
- Run `cmake`
- Close the cmd window

## Building

```
git clone https://github.com/powerof3/SPID-F4.git
cd SPID-F4
# pull commonlib /extern to override the path settings
git submodule update --init --recursive
```

### NG

```
cmake --preset vs2022-windows-vcpkg-ng
cmake --build build --config Release
```

### Pre-NG

```
cmake --preset vs2022-windows-vcpkg-f4
cmake --build buildf4 --config Release
```

### VR

```
cmake --preset vs2022-windows-vcpkg-vr
cmake --build buildvr --config Release
```

### All (one dll)

```
cmake --preset vs2022-windows-vcpkg-all
cmake --build buildall --config Release
```

## License

[MIT](LICENSE)

## Credits

[Skyrim SSE Version](https://github.com/powerof3/Spell-Perk-Item-Distributor)
