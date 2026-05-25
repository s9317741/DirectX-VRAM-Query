# DirectX-VRAM-Query

Windows utilities that query GPU video memory information using DirectX 9 / 11 / 12 APIs.

## Files

| File | API | VRAM Query Method |
|------|-----|-------------------|
| `main.cpp` | DirectX 9 | `GetAvailableTextureMem()` — estimated, not precise |
| `main_dx11.cpp` | DirectX 11 + DXGI 1.4 | `QueryVideoMemoryInfo()` — precise budget & usage |
| `main_dx12.cpp` | DirectX 12 + DXGI 1.6 | `QueryVideoMemoryInfo()` + `DXGI_ADAPTER_DESC3` |

## Output Example (DX11 / DX12)

```
GPU: NVIDIA GeForce RTX 3080
--- Local VRAM (GPU on-board) ---
  Budget:    9800 MB
  Used:      1024 MB
  Available: 8776 MB
--- Non-Local (Shared system memory) ---
  Budget:    8192 MB
  Used:       512 MB
```

## API Comparison

| | DX9 | DX11 | DX12 |
|---|---|---|---|
| Minimum Windows | XP | Vista | Windows 10 |
| Precise VRAM usage | No | Yes (DXGI 1.4) | Yes (DXGI 1.4) |
| GPU preference selection | No | No | Yes (DXGI 1.6) |
| Static capacity info | No | Partial | Yes (`DXGI_ADAPTER_DESC3`) |

## Requirements

- Windows 10 or later (for DX12 / DXGI 1.6)
- Windows SDK (includes `d3d9.h`, `d3d11.h`, `d3d12.h`, `dxgi1_6.h`)
- Visual Studio 2019 or later

## Build

### CMake (Recommended)

Requires CMake 3.20+ and Visual Studio 2022.

```cmd
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Binaries will be output to `build/Release/`:
- `vram_dx9.exe`
- `vram_dx11.exe`
- `vram_dx12.exe`

To build a single target:

```cmd
cmake --build build --config Release --target vram_dx11
```

### Command Line (MSVC Developer Prompt)

```cmd
# DirectX 9
cl main.cpp /link d3d9.lib

# DirectX 11
cl main_dx11.cpp /link d3d11.lib dxgi.lib

# DirectX 12
cl main_dx12.cpp /link d3d12.lib dxgi.lib
```

### Visual Studio

1. Create a new **Console Application** project
2. Add the desired `main*.cpp` file
3. Build in Release or Debug mode (libs are linked via `#pragma comment`)

## Known Limitations

- DX9's `GetAvailableTextureMem()` returns an estimated value only; use DX11/DX12 for accurate results
- `QueryVideoMemoryInfo()` reports the OS-managed budget, which may be lower than physical VRAM when other apps are running
