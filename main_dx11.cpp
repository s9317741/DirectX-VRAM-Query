#include <d3d11.h>
#include <dxgi1_4.h>
#include <stdio.h>
#include <windows.h>
#include <tchar.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

int main()
{
    // Create DXGI factory
    IDXGIFactory4* pFactory = NULL;
    if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&pFactory)))
    {
        printf("Failed to create DXGI factory.\n");
        return -1;
    }

    // Get default adapter (index 0)
    IDXGIAdapter3* pAdapter = NULL;
    {
        IDXGIAdapter1* pAdapter1 = NULL;
        if (FAILED(pFactory->EnumAdapters1(0, &pAdapter1)))
        {
            printf("Failed to enum adapters.\n");
            pFactory->Release();
            return -1;
        }
        if (FAILED(pAdapter1->QueryInterface(__uuidof(IDXGIAdapter3), (void**)&pAdapter)))
        {
            printf("Failed to get IDXGIAdapter3.\n");
            pAdapter1->Release();
            pFactory->Release();
            return -1;
        }
        pAdapter1->Release();
    }

    // Print GPU name
    DXGI_ADAPTER_DESC1 desc;
    if (FAILED(pAdapter->GetDesc1(&desc)))
    {
        printf("Failed to get adapter description.\n");
        pAdapter->Release();
        pFactory->Release();
        return -1;
    }
    printf("GPU: %ls\n", desc.Description);

    // Create D3D11 device so DXGI can report accurate memory usage
    ID3D11Device* pDevice = NULL;
    D3D_FEATURE_LEVEL featureLevel;
    if (FAILED(D3D11CreateDevice(pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, 0,
                                 NULL, 0, D3D11_SDK_VERSION, &pDevice, &featureLevel, NULL)))
    {
        printf("Failed to create D3D11 device.\n");
        pAdapter->Release();
        pFactory->Release();
        return -1;
    }

    // Query precise VRAM usage via DXGI 1.4
    DXGI_QUERY_VIDEO_MEMORY_INFO localInfo = {};
    DXGI_QUERY_VIDEO_MEMORY_INFO nonLocalInfo = {};
    pAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &localInfo);
    pAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &nonLocalInfo);

    printf("--- Local VRAM (GPU on-board) ---\n");
    printf("  Budget:   %llu MB\n", localInfo.Budget / 1024 / 1024);
    printf("  Used:     %llu MB\n", localInfo.CurrentUsage / 1024 / 1024);
    printf("  Available:%llu MB\n", (localInfo.Budget - localInfo.CurrentUsage) / 1024 / 1024);

    printf("--- Non-Local (Shared system memory) ---\n");
    printf("  Budget:   %llu MB\n", nonLocalInfo.Budget / 1024 / 1024);
    printf("  Used:     %llu MB\n", nonLocalInfo.CurrentUsage / 1024 / 1024);

    pDevice->Release();
    pAdapter->Release();
    pFactory->Release();

    return 0;
}
