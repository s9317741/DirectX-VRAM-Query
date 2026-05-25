#include <d3d12.h>
#include <dxgi1_6.h>
#include <stdio.h>
#include <windows.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

int main()
{
    // Create DXGI factory (Factory6 supports GPU preference enumeration)
    IDXGIFactory6* pFactory = NULL;
    if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory6), (void**)&pFactory)))
    {
        printf("Failed to create DXGI factory.\n");
        return -1;
    }

    // Prefer high-performance GPU (discrete over integrated)
    IDXGIAdapter4* pAdapter = NULL;
    if (FAILED(pFactory->EnumAdapterByGpuPreference(
            0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
            __uuidof(IDXGIAdapter4), (void**)&pAdapter)))
    {
        printf("Failed to enum adapters.\n");
        pFactory->Release();
        return -1;
    }

    // Print GPU name
    DXGI_ADAPTER_DESC3 desc;
    if (FAILED(pAdapter->GetDesc3(&desc)))
    {
        printf("Failed to get adapter description.\n");
        pAdapter->Release();
        pFactory->Release();
        return -1;
    }
    printf("GPU: %ls\n", desc.Description);

    // Create D3D12 device
    ID3D12Device* pDevice = NULL;
    if (FAILED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&pDevice)))
    {
        printf("Failed to create D3D12 device.\n");
        pAdapter->Release();
        pFactory->Release();
        return -1;
    }

    // Query precise VRAM usage via DXGI 1.4 (IDXGIAdapter3 interface)
    IDXGIAdapter3* pAdapter3 = NULL;
    if (FAILED(pAdapter->QueryInterface(__uuidof(IDXGIAdapter3), (void**)&pAdapter3)))
    {
        printf("Failed to get IDXGIAdapter3.\n");
        pDevice->Release();
        pAdapter->Release();
        pFactory->Release();
        return -1;
    }

    DXGI_QUERY_VIDEO_MEMORY_INFO localInfo = {};
    DXGI_QUERY_VIDEO_MEMORY_INFO nonLocalInfo = {};
    pAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &localInfo);
    pAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &nonLocalInfo);

    printf("--- Local VRAM (GPU on-board) ---\n");
    printf("  Budget:   %llu MB\n", localInfo.Budget / 1024 / 1024);
    printf("  Used:     %llu MB\n", localInfo.CurrentUsage / 1024 / 1024);
    printf("  Available:%llu MB\n", (localInfo.Budget - localInfo.CurrentUsage) / 1024 / 1024);

    printf("--- Non-Local (Shared system memory) ---\n");
    printf("  Budget:   %llu MB\n", nonLocalInfo.Budget / 1024 / 1024);
    printf("  Used:     %llu MB\n", nonLocalInfo.CurrentUsage / 1024 / 1024);

    // DXGI_ADAPTER_DESC3 reports static VRAM capacity from the driver (not real-time usage)
    printf("--- DXGI_ADAPTER_DESC3 (from driver) ---\n");
    printf("  DedicatedVideoMemory:  %llu MB\n", desc.DedicatedVideoMemory / 1024 / 1024);
    printf("  DedicatedSystemMemory: %llu MB\n", desc.DedicatedSystemMemory / 1024 / 1024);
    printf("  SharedSystemMemory:    %llu MB\n", desc.SharedSystemMemory / 1024 / 1024);

    pAdapter3->Release();
    pDevice->Release();
    pAdapter->Release();
    pFactory->Release();

    return 0;
}
