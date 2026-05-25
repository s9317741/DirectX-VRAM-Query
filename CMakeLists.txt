cmake_minimum_required(VERSION 3.20)
project(DirectX-VRAM-Query CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT WIN32)
    message(FATAL_ERROR "DirectX targets require Windows. Only Vulkan and OpenCL targets are cross-platform.")
endif()

# DirectX 9
add_executable(vram_dx9 main.cpp)
target_link_libraries(vram_dx9 PRIVATE d3d9)

# DirectX 11
add_executable(vram_dx11 main_dx11.cpp)
target_link_libraries(vram_dx11 PRIVATE d3d11 dxgi)

# DirectX 12
add_executable(vram_dx12 main_dx12.cpp)
target_link_libraries(vram_dx12 PRIVATE d3d12 dxgi)

# Vulkan (requires Vulkan SDK: https://vulkan.lunarg.com/)
find_package(Vulkan QUIET)
if(Vulkan_FOUND)
    add_executable(vram_vulkan main_vulkan.cpp)
    target_link_libraries(vram_vulkan PRIVATE Vulkan::Vulkan)
    message(STATUS "Vulkan found: vram_vulkan target enabled")
else()
    message(STATUS "Vulkan SDK not found: skipping vram_vulkan target")
endif()

# OpenCL (requires OpenCL SDK: CUDA Toolkit, Intel oneAPI, or AMD ROCm)
find_package(OpenCL QUIET)
if(OpenCL_FOUND)
    add_executable(vram_opencl main_opencl.cpp)
    target_link_libraries(vram_opencl PRIVATE OpenCL::OpenCL)
    message(STATUS "OpenCL found: vram_opencl target enabled")
else()
    message(STATUS "OpenCL not found: skipping vram_opencl target")
endif()
