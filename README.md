# DirectX-VRAM-Query

Windows utilities that query GPU video memory information using DirectX 9 / 11 / 12, Vulkan, and OpenCL APIs.

![Build](https://github.com/s9317741/DirectX-VRAM-Query/actions/workflows/build.yml/badge.svg)

## Files

| File | API | Platform | VRAM Query Method |
|------|-----|----------|-------------------|
| `main.cpp` | DirectX 9 | Windows | `GetAvailableTextureMem()` — estimated, not precise |
| `main_dx11.cpp` | DirectX 11 + DXGI 1.4 | Windows | `QueryVideoMemoryInfo()` — precise budget & usage |
| `main_dx12.cpp` | DirectX 12 + DXGI 1.6 | Windows | `QueryVideoMemoryInfo()` + `DXGI_ADAPTER_DESC3` |
| `main_vulkan.cpp` | Vulkan 1.0 | Windows / Linux / macOS | `VkPhysicalDeviceMemoryProperties` |
| `main_opencl.cpp` | OpenCL | Windows / Linux / macOS | `CL_DEVICE_GLOBAL_MEM_SIZE` |

## Features

- Lists **all GPUs** on the system, not just the primary one
- `--json` flag outputs valid JSON for scripting and automation
- `--watch` flag refreshes every second (like `watch` on Linux)

## Output Example

```
[GPU 0] NVIDIA GeForce RTX 3080
  Local VRAM  - Budget: 9800 MB  Used: 1024 MB  Available: 8776 MB
  Non-Local   - Budget: 8192 MB  Used: 512 MB

[GPU 1] Intel(R) UHD Graphics 770
  Local VRAM  - Budget: 128 MB  Used: 64 MB  Available: 64 MB
  Non-Local   - Budget: 8192 MB  Used: 1024 MB
```

### JSON output (`--json`)

```json
[
  {
    "index": 0,
    "name": "NVIDIA GeForce RTX 3080",
    "local": {
      "budget_mb": 9800,
      "used_mb": 1024,
      "available_mb": 8776
    },
    "non_local": {
      "budget_mb": 8192,
      "used_mb": 512
    }
  }
]
```

## API Comparison

| | DX9 | DX11 | DX12 | Vulkan | OpenCL |
|---|---|---|---|---|---|
| Minimum Windows | XP | Vista | Windows 10 | Windows 10 | Windows 7 |
| Cross-platform | No | No | No | Yes | Yes |
| Precise VRAM usage | No | Yes | Yes | Heap size only | Heap size only |
| All GPUs | No | Yes | Yes | Yes | Yes |
| GPU preference sort | No | No | Yes | No | No |
| `--json` / `--watch` | No | Yes | Yes | Yes | Yes |

## Requirements

### DirectX targets (DX9 / DX11 / DX12)
- Windows 10 or later (for DX12 / DXGI 1.6)
- Windows SDK (included with Visual Studio)

### Vulkan target
- [Vulkan SDK](https://vulkan.lunarg.com/) installed

### OpenCL target
- OpenCL SDK from one of: [CUDA Toolkit](https://developer.nvidia.com/cuda-downloads), [Intel oneAPI](https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit.html), or [AMD ROCm](https://rocm.docs.amd.com/)

## Build

### CMake (Recommended)

Requires CMake 3.20+ and Visual Studio 2022.

```cmd
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Vulkan and OpenCL targets are built automatically if their SDKs are detected. Binaries are output to `build/Release/`.

To build a single target:

```cmd
cmake --build build --config Release --target vram_dx12
```

### Command Line (MSVC Developer Prompt)

```cmd
cl main.cpp /link d3d9.lib
cl main_dx11.cpp /link d3d11.lib dxgi.lib
cl main_dx12.cpp /link d3d12.lib dxgi.lib
cl main_vulkan.cpp /I"%VULKAN_SDK%\Include" /link /LIBPATH:"%VULKAN_SDK%\Lib" vulkan-1.lib
cl main_opencl.cpp /I"%OPENCL_ROOT%\include" /link /LIBPATH:"%OPENCL_ROOT%\lib\x64" OpenCL.lib
```

## Usage

```cmd
vram_dx12.exe              # default output, all GPUs
vram_dx12.exe --json       # JSON output
vram_dx12.exe --watch      # refresh every second
vram_dx12.exe --json --watch | your-script.py
```

## Known Limitations

- DX9's `GetAvailableTextureMem()` returns an estimated value; use DX11/DX12 for accurate results
- `QueryVideoMemoryInfo()` reports the OS-managed budget, which may be lower than physical VRAM when other apps are running
- Vulkan and OpenCL report heap capacity, not real-time usage (no equivalent of `QueryVideoMemoryInfo` in those APIs)

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).
