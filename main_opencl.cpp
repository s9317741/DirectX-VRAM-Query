#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Link: requires OpenCL SDK (included in CUDA Toolkit, Intel oneAPI, or AMD ROCm)
// MSVC: cl main_opencl.cpp /I"%OPENCL_ROOT%\include" /link /LIBPATH:"%OPENCL_ROOT%\lib\x64" OpenCL.lib
// CMake: target_link_libraries(vram_opencl PRIVATE OpenCL::OpenCL)

static void print_device(cl_device_id device, cl_uint platformIndex, cl_uint deviceIndex, bool json)
{
    char name[256] = {};
    char vendor[128] = {};
    cl_device_type deviceType;
    cl_ulong globalMem = 0;
    cl_ulong localMem = 0;
    cl_ulong maxAllocSize = 0;

    clGetDeviceInfo(device, CL_DEVICE_NAME,            sizeof(name),          name,          NULL);
    clGetDeviceInfo(device, CL_DEVICE_VENDOR,          sizeof(vendor),        vendor,        NULL);
    clGetDeviceInfo(device, CL_DEVICE_TYPE,            sizeof(deviceType),    &deviceType,   NULL);
    clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(globalMem),     &globalMem,    NULL);
    clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE,  sizeof(localMem),      &localMem,     NULL);
    clGetDeviceInfo(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(maxAllocSize), &maxAllocSize, NULL);

    const char* typeStr = "Unknown";
    if (deviceType & CL_DEVICE_TYPE_GPU)         typeStr = "GPU";
    else if (deviceType & CL_DEVICE_TYPE_CPU)    typeStr = "CPU";
    else if (deviceType & CL_DEVICE_TYPE_ACCELERATOR) typeStr = "Accelerator";

    if (json)
    {
        printf("  {\n");
        printf("    \"platform\": %u,\n", platformIndex);
        printf("    \"device\": %u,\n", deviceIndex);
        printf("    \"name\": \"%s\",\n", name);
        printf("    \"vendor\": \"%s\",\n", vendor);
        printf("    \"type\": \"%s\",\n", typeStr);
        printf("    \"global_mem_mb\": %llu,\n", globalMem / 1024 / 1024);
        printf("    \"local_mem_kb\": %llu,\n", localMem / 1024);
        printf("    \"max_alloc_mb\": %llu\n", maxAllocSize / 1024 / 1024);
        printf("  }");
    }
    else
    {
        printf("[Platform %u / Device %u] %s (%s) - %s\n", platformIndex, deviceIndex, name, vendor, typeStr);
        printf("  Global Memory  : %llu MB\n", globalMem / 1024 / 1024);
        printf("  Local Memory   : %llu KB\n", localMem / 1024);
        printf("  Max Alloc Size : %llu MB\n\n", maxAllocSize / 1024 / 1024);
    }
}

static void query_all(bool json)
{
    cl_uint platformCount = 0;
    clGetPlatformIDs(0, NULL, &platformCount);
    if (platformCount == 0)
    {
        printf("No OpenCL platform found.\n");
        return;
    }

    cl_platform_id* platforms = new cl_platform_id[platformCount];
    clGetPlatformIDs(platformCount, platforms, NULL);

    if (json) printf("[\n");
    bool firstEntry = true;

    for (cl_uint p = 0; p < platformCount; p++)
    {
        cl_uint deviceCount = 0;
        clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
        if (deviceCount == 0) continue;

        cl_device_id* devices = new cl_device_id[deviceCount];
        clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL, deviceCount, devices, NULL);

        for (cl_uint d = 0; d < deviceCount; d++)
        {
            if (json && !firstEntry) printf(",\n");
            print_device(devices[d], p, d, json);
            firstEntry = false;
        }

        delete[] devices;
    }

    if (json) printf("\n]\n");
    delete[] platforms;
}

int main(int argc, char* argv[])
{
    bool json = false;
    bool watch = false;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--json") == 0)   json = true;
        if (strcmp(argv[i], "--watch") == 0)  watch = true;
    }

    do
    {
#ifdef _WIN32
        if (watch && !json) system("cls");
#else
        if (watch && !json) system("clear");
#endif
        query_all(json);
#ifdef _WIN32
        if (watch) Sleep(1000);
#else
        if (watch) sleep(1);
#endif
    } while (watch);

    return 0;
}
