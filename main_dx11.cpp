#include <d3d11.h>
#include <dxgi1_4.h>
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <string.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

static void print_adapter(IDXGIAdapter3* pAdapter, UINT index, bool json)
{
    DXGI_ADAPTER_DESC1 desc;
    if (FAILED(pAdapter->GetDesc1(&desc)))
        return;

    DXGI_QUERY_VIDEO_MEMORY_INFO localInfo = {};
    DXGI_QUERY_VIDEO_MEMORY_INFO nonLocalInfo = {};
    pAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &localInfo);
    pAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &nonLocalInfo);

    if (json)
    {
        printf("  {\n");
        printf("    \"index\": %u,\n", index);
        printf("    \"name\": \"%ls\",\n", desc.Description);
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
        printf("  Local VRAM  - Budget: %llu MB  Used: %llu MB  Available: %llu MB\n",
               localInfo.Budget / 1024 / 1024,
               localInfo.CurrentUsage / 1024 / 1024,
               (localInfo.Budget - localInfo.CurrentUsage) / 1024 / 1024);
        printf("  Non-Local   - Budget: %llu MB  Used: %llu MB\n",
               nonLocalInfo.Budget / 1024 / 1024,
               nonLocalInfo.CurrentUsage / 1024 / 1024);
    }
}

static void query_all(IDXGIFactory4* pFactory, bool json)
{
    if (json) printf("[\n");

    IDXGIAdapter1* pAdapter1 = NULL;
    UINT adapterIndex = 0;
    bool first = true;

    while (pFactory->EnumAdapters1(adapterIndex, &pAdapter1) != DXGI_ERROR_NOT_FOUND)
    {
        IDXGIAdapter3* pAdapter3 = NULL;
        if (SUCCEEDED(pAdapter1->QueryInterface(__uuidof(IDXGIAdapter3), (void**)&pAdapter3)))
        {
            // Create device so DXGI reports accurate memory usage
            ID3D11Device* pDevice = NULL;
            D3D_FEATURE_LEVEL featureLevel;
            D3D11CreateDevice(pAdapter3, D3D_DRIVER_TYPE_UNKNOWN, NULL, 0,
                              NULL, 0, D3D11_SDK_VERSION, &pDevice, &featureLevel, NULL);

            if (json && !first) printf(",\n");
            print_adapter(pAdapter3, adapterIndex, json);
            if (!json) printf("\n");
            first = false;

            if (pDevice) pDevice->Release();
            pAdapter3->Release();
        }
        pAdapter1->Release();
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

    IDXGIFactory4* pFactory = NULL;
    if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&pFactory)))
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
