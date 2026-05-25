#include <d3d9.h>
#include <stdio.h>
#include <windows.h>
#include <tchar.h>

#pragma comment(lib, "d3d9.lib")

int main()
{
    // Register window class
    WNDCLASSEX wc = {};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = DefWindowProc;
    wc.hInstance     = GetModuleHandle(NULL);
    wc.lpszClassName = _T("MyWindowClass");
    if (!RegisterClassEx(&wc))
    {
        printf("Failed to register window class.\n");
        return -1;
    }

    HINSTANCE hInstance = GetModuleHandle(NULL);
    HWND hWnd = CreateWindowEx(0, _T("MyWindowClass"), _T("My Window"), WS_OVERLAPPEDWINDOW,
                               0, 0, 640, 480, NULL, NULL, hInstance, NULL);
    if (hWnd == NULL)
    {
        printf("Failed to create window.\n");
        UnregisterClass(_T("MyWindowClass"), hInstance);
        return -1;
    }

    ShowWindow(hWnd, SW_SHOW);

    IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!pD3D)
    {
        printf("Failed to create IDirect3D9 object.\n");
        DestroyWindow(hWnd);
        return -1;
    }

    D3DADAPTER_IDENTIFIER9 adapterId;
    ZeroMemory(&adapterId, sizeof(D3DADAPTER_IDENTIFIER9));
    if (FAILED(pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &adapterId)))
    {
        printf("Failed to get adapter identifier.\n");
        pD3D->Release();
        DestroyWindow(hWnd);
        return -1;
    }

    printf("GPU: %s\n", adapterId.Description);

    D3DDEVTYPE deviceType = D3DDEVTYPE_HAL;

    D3DPRESENT_PARAMETERS presentParams = {};
    presentParams.Windowed               = TRUE;
    presentParams.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    presentParams.BackBufferFormat       = D3DFMT_UNKNOWN;
    presentParams.BackBufferCount        = 1;
    presentParams.EnableAutoDepthStencil = TRUE;
    presentParams.AutoDepthStencilFormat = D3DFMT_D24S8;
    presentParams.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

    IDirect3DDevice9* pDevice = NULL;
    if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, deviceType, hWnd,
                                  D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentParams, &pDevice)))
    {
        printf("Failed to create D3D device.\n");
        pD3D->Release();
        DestroyWindow(hWnd);
        return -1;
    }

    pD3D->Release();

    UINT availableVram = pDevice->GetAvailableTextureMem() / 1024 / 1024;
    printf("Available VRAM: %u MB\n", availableVram);

    pDevice->Release();
    DestroyWindow(hWnd);

    return 0;
}
