#include <vulkan/vulkan.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Link: requires Vulkan SDK installed (https://vulkan.lunarg.com/)
// MSVC: cl main_vulkan.cpp /I"%VULKAN_SDK%\Include" /link /LIBPATH:"%VULKAN_SDK%\Lib" vulkan-1.lib
// CMake: target_link_libraries(vram_vulkan PRIVATE Vulkan::Vulkan)

static void print_device(VkPhysicalDevice device, uint32_t index, bool json)
{
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(device, &props);

    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(device, &memProps);

    // Sum up DEVICE_LOCAL heaps (VRAM) and non-device-local heaps (shared)
    uint64_t localBytes = 0;
    uint64_t nonLocalBytes = 0;
    for (uint32_t i = 0; i < memProps.memoryHeapCount; i++)
    {
        if (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
            localBytes += memProps.memoryHeaps[i].size;
        else
            nonLocalBytes += memProps.memoryHeaps[i].size;
    }

    const char* deviceType = "Unknown";
    switch (props.deviceType)
    {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   deviceType = "Discrete GPU";   break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: deviceType = "Integrated GPU"; break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    deviceType = "Virtual GPU";    break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:            deviceType = "CPU";            break;
        default: break;
    }

    if (json)
    {
        printf("  {\n");
        printf("    \"index\": %u,\n", index);
        printf("    \"name\": \"%s\",\n", props.deviceName);
        printf("    \"type\": \"%s\",\n", deviceType);
        printf("    \"local_vram_mb\": %llu,\n", localBytes / 1024 / 1024);
        printf("    \"non_local_mb\": %llu\n", nonLocalBytes / 1024 / 1024);
        printf("  }");
    }
    else
    {
        printf("[GPU %u] %s (%s)\n", index, props.deviceName, deviceType);
        printf("  Device-Local VRAM : %llu MB\n", localBytes / 1024 / 1024);
        printf("  Non-Local Memory  : %llu MB\n\n", nonLocalBytes / 1024 / 1024);
    }
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

    VkApplicationInfo appInfo = {};
    appInfo.sType      = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkInstance instance;
    if (vkCreateInstance(&createInfo, NULL, &instance) != VK_SUCCESS)
    {
        printf("Failed to create Vulkan instance.\n");
        return -1;
    }

    do
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
        if (deviceCount == 0)
        {
            printf("No Vulkan-capable GPU found.\n");
            break;
        }

        VkPhysicalDevice* devices = new VkPhysicalDevice[deviceCount];
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

        if (json) printf("[\n");
        for (uint32_t i = 0; i < deviceCount; i++)
        {
            if (json && i > 0) printf(",\n");
            print_device(devices[i], i, json);
        }
        if (json) printf("\n]\n");

        delete[] devices;

#ifdef _WIN32
        if (watch) Sleep(1000);
#else
        if (watch) sleep(1);
#endif
    } while (watch);

    vkDestroyInstance(instance, NULL);
    return 0;
}
