cmake_minimum_required(VERSION 3.20)
project(DirectX-VRAM-Query CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT WIN32)
    message(FATAL_ERROR "This project requires Windows.")
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
