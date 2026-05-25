#include <d3d12.h>
#include <dxgi1_6.h>
#include <stdio.h>
#include <windows.h>
#include <string.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

static void print_adapter(IDXGIAdapter4* pAdapter, UINT index, bool json)
{
    DXGI_ADAPTER_DESC3 desc;
    if (FAILED(pAdapter->GetDesc3(&desc)))
        return;

    IDXGIAdapter3* pAdapter3 = NULL;
    if (FAILED(pAdapter->QueryInterface(__uuidof(IDXGIAdapter3), (void**)&pAdapter3)))
        return;

    DXGI_QUERY_VIDEO_MEMORY_INFO localInfo = {};
    DXGI_QUERY_VIDEO_MEMORY_INFO nonLocalInfo = {};
    pAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &localInfo);
    pAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &nonLocalInfo);
    pAdapter3->Release();

    if (json)
    {
        printf("  {\n");
        printf("    \"index\": %u,\n", index);
        printf("    \"name\": \"%ls\",\n", desc.Description);
        printf("    \"dedicated_video_mb\": %llu,\n", desc.DedicatedVideoMemory / 1024 / 1024);
        printf("    \"dedicated_system_mb\": %llu,\n", desc.DedicatedSystemMemory / 1024 / 1024);
        printf("    \"shared_system_mb\": %llu,\n", desc.SharedSystemMemory / 1024 / 1024);
        printf("    \"local\": {\n");
        printf("      \"budget_mb\": %llu,\n", localInfo.Budget / 1024 / 1024);
        printf("      \"used_mb\": %llu,\n", localInfo.CurrentUsage / 1024 / 1024);
        printf("      \"available_mb\": %llu\n", (localInfo.Budget - localInfo.CurrentUsage) / 1024 / 1024);
        printf("    },\n");
        printf("    \"non_local\": {\n");
        printf("      \"budget_mb\": %llu,\n", nonLocalInfo.Budget / 1024 / 1024);
        printf("      \"used_mb\": %llu\n", nonLocalInfo.CurrentUsage / 1024 / 1024);
        printf("    }\n");
        printf("  }");
    }
    else
    {
        printf("[GPU %u] %ls\n", index, desc.Description);
        printf("  Dedicated VRAM : %llu MB\n", desc.DedicatedVideoMemory / 1024 / 1024);
        printf("  Local VRAM     - Budget: %llu MB  Used: %llu MB  Available: %llu MB\n",
               localInfo.Budget / 1024 / 1024,
               localInfo.CurrentUsage / 1024 / 1024,
               (localInfo.Budget - localInfo.CurrentUsage) / 1024 / 1024);
        printf("  Non-Local      - Budget: %llu MB  Used: %llu MB\n",
               nonLocalInfo.Budget / 1024 / 1024,
               nonLocalInfo.CurrentUsage / 1024 / 1024);
    }
}

static void query_all(IDXGIFactory6* pFactory, bool json)
{
    if (json) printf("[\n");

    IDXGIAdapter4* pAdapter = NULL;
    UINT adapterIndex = 0;
    bool first = true;

    while (pFactory->EnumAdapterByGpuPreference(
               adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
               __uuidof(IDXGIAdapter4), (void**)&pAdapter) != DXGI_ERROR_NOT_FOUND)
    {
        // Create device so DXGI reports accurate memory usage
        ID3D12Device* pDevice = NULL;
        D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&pDevice);

        if (json && !first) printf(",\n");
        print_adapter(pAdapter, adapterIndex, json);
        if (!json) printf("\n");
        first = false;

        if (pDevice) pDevice->Release();
        pAdapter->Release();
        adapterIndex++;
    }

    if (json) printf("\n]\n");
}

int main(int argc, char* argv[])
{
    bool json = false;
    bool watch = false;
    int interval = 1000;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--json") == 0)   json = true;
        if (strcmp(argv[i], "--watch") == 0)  watch = true;
    }

    IDXGIFactory6* pFactory = NULL;
    if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory6), (void**)&pFactory)))
    {
        printf("Failed to create DXGI factory.\n");
        return -1;
    }

    do
    {
        if (watch && !json) system("cls");
        query_all(pFactory, json);
        if (watch) Sleep(interval);
    } while (watch);

    pFactory->Release();
    return 0;
}
