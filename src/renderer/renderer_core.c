#include "renderer_core.h"

#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>

#define DEVICE_QUEUE_COUNT 1

static VkSurfaceKHR _create_window_surface(VkInstance instance, GLFWwindow *window);
static VkInstance _create_instance(uint32_t extension_count, const char **extensions, uint32_t layer_count, const char **layers);
static VkPhysicalDevice _select_physical_device(uint32_t count, VkPhysicalDevice *physical_devices);
static uint32_t _get_suitable_queue_family_index(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
static VkDevice _create_logical_device(uint32_t queue_idx_count, uint32_t *queue_idxs, uint32_t extension_count, const char **extensions, VkPhysicalDevice physical_device);

void rc_initialize(RendererCore *renderer_core, const RendererCoreInit *init, Window window)
{
    VkInstance instance = _create_instance(
            init->instance_extension_count, init->instance_extensions, 
            init->layer_count, init->layers
    );
    
    GLFWwindow **pwindow = (GLFWwindow**)window;
    VkSurfaceKHR surface = _create_window_surface(instance, *pwindow);

    uint32_t physical_device_count = 0;
    VkPhysicalDevice *physical_devices = NULL;
    vkEnumeratePhysicalDevices(instance, &physical_device_count, NULL);
    physical_devices = calloc(sizeof(VkPhysicalDevice), physical_device_count);
    vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices);
    VkPhysicalDevice physical_device = _select_physical_device(physical_device_count, physical_devices);
    free(physical_devices);

    uint32_t graphics_family_idx = _get_suitable_queue_family_index(physical_device, surface);

    VkDevice device = _create_logical_device(1, &graphics_family_idx, init->device_extension_count, init->device_extensions, physical_device);

    renderer_core->device = device;
    renderer_core->physical_device = physical_device;
    renderer_core->graphics_family_idx = graphics_family_idx;
    renderer_core->surface = surface;
    renderer_core->instance = instance;
}

void rc_destroy(const RendererCore *renderer_core)
{
    vkDestroyDevice(renderer_core->device, NULL);
    vkDestroyInstance(renderer_core->instance, NULL);
}

static VkInstance _create_instance(uint32_t extension_count, const char **extensions, uint32_t layer_count, const char **layers)
{
    VkInstance instance;
    
    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_4,
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .pApplicationName = "Sandbox",
        .pEngineName = "Scorching Sands"
    };

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = extension_count,
        .ppEnabledExtensionNames = extensions,
        .enabledLayerCount = layer_count,
        .ppEnabledLayerNames = layers,
    };

    VkResult result = vkCreateInstance(
                        &createInfo, 
                        NULL, 
                        &instance
                    );

    VK_RESULT_CHECK(result);

    return instance;
}

static VkPhysicalDevice _select_physical_device(uint32_t count, VkPhysicalDevice *physical_devices)
{
    return physical_devices[0];
}

static uint32_t _get_suitable_queue_family_index(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
    uint32_t graphics_family_idx = -1;

    uint32_t count = 0;
    VkQueueFamilyProperties *props = NULL;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, NULL);
    props = (VkQueueFamilyProperties *)calloc(sizeof(VkQueueFamilyProperties), count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, props);
   
    for (uint32_t i = 0; i < count; ++i)
    {
        VkQueueFamilyProperties family = props[i];
        VkBool32 presentation_supported = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &presentation_supported);
        
        if (presentation_supported && (family.queueFlags & VK_QUEUE_GRAPHICS_BIT))
        {
            graphics_family_idx = i;
            break;
        }
    }

    free(props);
    return graphics_family_idx;
}

static VkDevice _create_logical_device(uint32_t queue_idx_count, uint32_t *queue_idxs, uint32_t extension_count, const char **extensions, VkPhysicalDevice physical_device)
{
    VkDevice device = NULL;
    VkDeviceQueueCreateInfo *infos = calloc(queue_idx_count, sizeof(VkDeviceQueueCreateInfo)); 
    float priority = 1.0f;

    for (uint32_t i = 0; i < queue_idx_count; ++i)
    {
        infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        infos[i].queueCount = DEVICE_QUEUE_COUNT;
        infos[i].queueFamilyIndex = queue_idxs[i];
        infos[i].pQueuePriorities = &priority;
    }

    VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .enabledExtensionCount = extension_count,
        .ppEnabledExtensionNames = extensions,
        .pQueueCreateInfos = infos,
        .queueCreateInfoCount = queue_idx_count
    };
    
    VkResult result = vkCreateDevice(physical_device, &createInfo, NULL, &device);
    VK_RESULT_CHECK(result);

    return device;
}

static VkSurfaceKHR _create_window_surface(VkInstance instance, GLFWwindow *window)
{
    VkSurfaceKHR surface;

    if (glfwCreateWindowSurface(instance, window, NULL, &surface) != VK_SUCCESS) 
    {
        return NULL;
    }

    return surface;
}
