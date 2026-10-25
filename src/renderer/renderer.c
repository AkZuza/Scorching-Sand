#include "renderer.h"
#include "renderer/renderer_core.h"
#include <stdint.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>
#include <window/window.h>
#include <log/log.h>
#include <stdlib.h>
#include <stdio.h>

#define GLFW_PLATFORM_WAYLAND
#define GLFW_INCLUDE_VULKAN

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

Renderer *g_renderer = NULL;

#define MAX_INFLIGHT_FRAMES 3

typedef struct RendererState {
    uint32_t            image_idx;
    uint32_t            frame_idx;
    uint32_t            swapchainImagesCount;
    VkCommandPool       commandPool;
    VkCommandBuffer     primaryCommandBuffers[MAX_INFLIGHT_FRAMES];
    VkSwapchainKHR      swapchain;
    VkImage             *swapchainImages;
    VkImageView         *swapchainImageViews;
    VkExtent2D          swapchainExtent;
    VkFormat            swapchainFormat;
    VkPresentModeKHR    swapchainPresentMode;
    VkPipeline          graphicsPipeline;
    VkQueue             submitQueue;
    VkSemaphore         presentSemaphores[MAX_INFLIGHT_FRAMES];
    VkSemaphore         *renderingSemaphores;
    VkFence             drawFences[MAX_INFLIGHT_FRAMES];
    VkFence             acquireFences[MAX_INFLIGHT_FRAMES];
} RendererState;

void _create_basic_renderer(Renderer const *renderer);

Renderer* r_create_renderer(const Window window)
{
    if (g_renderer != NULL)
    {
        LOG_WARN("Renderer creation: Creating more than instance is invalid\n");
        return NULL;
    }

    Renderer* renderer = malloc(sizeof(Renderer) + sizeof(RendererState));
    RendererState *state = (RendererState *)(renderer + 1);

    state->frame_idx = 0;
    state->image_idx = 0;
    renderer->window = window;
    g_renderer = renderer;
    return renderer;
}

void r_initialize(Renderer *renderer)
{
    const uint32_t instance_extension_count = 3;
    const char * instance_extensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        "VK_KHR_wayland_surface",
        "VK_KHR_get_surface_capabilities2"
    };

    const uint32_t device_extenstion_count = 3;
    const char *device_extenstions[] = {
        "VK_KHR_swapchain",
        "VK_KHR_dynamic_rendering",
        "VK_KHR_synchronization2",
        "VK_KHR_robustness2",
    };

    const RendererCoreInit init = {
        .instance_extension_count = instance_extension_count,
        .instance_extensions = instance_extensions,
        .device_extension_count = device_extenstion_count,
        .device_extensions = device_extenstions
    };

    rc_initialize(&renderer->core, &init, renderer->window);

    _create_basic_renderer(renderer);
}

void r_destroy(const Renderer *renderer)
{
    rc_destroy(&renderer->core);
    free((void*)renderer);
}

const Renderer* r_get_renderer()
{
    return g_renderer;
}

void r_begin_frame()
{
    const Renderer *renderer = r_get_renderer();
    VkDevice device = renderer->core.device;
    RendererState* state = (RendererState *)(renderer + 1);

    uint32_t frame_idx = state->frame_idx;
    vkWaitForFences(device, 1, &state->drawFences[frame_idx], VK_TRUE, UINT64_MAX);

    VkAcquireNextImageInfoKHR acquireInfo = {
        .sType = VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR,
        .timeout = UINT64_MAX,
        .swapchain = state->swapchain,
        .semaphore = state->presentSemaphores[frame_idx],
        // .fence = state->acquireFences[frame_idx],
        .deviceMask = 1,
    };
    vkAcquireNextImage2KHR(device, &acquireInfo, &(state->image_idx));
    // vkWaitForFences(device, 1, state->acquireFences + frame_idx, VK_TRUE, UINT64_MAX);
    uint32_t image_index = state->image_idx;

    // Reset fences
    VkFence fences[] = { state->drawFences[frame_idx], state->acquireFences[frame_idx] };
    vkResetFences(device, 1, fences);

    static VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };
    VkCommandBuffer commandBuffer = state->primaryCommandBuffers[frame_idx];
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    // Image Layout Transition
    VkImageMemoryBarrier2 barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .image = state->swapchainImages[image_index],
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseArrayLayer = 0,
            .baseMipLevel = 0,
            .layerCount = 1,
            .levelCount = 1,
        }
    };

    VkDependencyInfo dependencyInfo = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &barrier,
    };

    vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);

    VkClearValue clear_value = {
        .color = { {0.8f, 0.5f, 0.6f, 1.0f} },
    };

    VkRenderingAttachmentInfo attachmentInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .clearValue = clear_value,
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .imageView = state->swapchainImageViews[image_index],
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE
    };

    VkRenderingInfo renderingInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentInfo,
        .layerCount = 1,
        .renderArea = { .offset = {0, 0}, .extent = state->swapchainExtent },
    };

    vkCmdBeginRendering(commandBuffer, &renderingInfo);

    // Test code here
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, state->graphicsPipeline);

    VkViewport viewport = { 0.0f, 0.0f, (float)state->swapchainExtent.width, (float)state->swapchainExtent.height, 0.0f, 1.0f };
    VkRect2D scissor = { .offset = {0, 0}, .extent = state->swapchainExtent };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void r_end_frame()
{
    const Renderer *renderer = r_get_renderer();
    VkDevice device = renderer->core.device;
    RendererState* state = (RendererState *)(renderer + 1);
    uint32_t image_index = state->image_idx;
    uint32_t frame_idx = state->frame_idx;
    VkCommandBuffer commandBuffer = state->primaryCommandBuffers[frame_idx];

    // Image Layout Transition
    VkImageMemoryBarrier2 barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .image = state->swapchainImages[image_index],
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseArrayLayer = 0,
            .baseMipLevel = 0,
            .layerCount = 1,
            .levelCount = 1,
        }
    };

    VkDependencyInfo dependencyInfo = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &barrier,
    };

    vkCmdEndRendering(commandBuffer);
    vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);


    vkEndCommandBuffer(commandBuffer);

    VkCommandBufferSubmitInfo commandBufferSubmitInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .commandBuffer = commandBuffer,
        .deviceMask = 1
    };

    VkSemaphoreSubmitInfo presentSemaphoreWaitInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = state->presentSemaphores[frame_idx],
        .deviceIndex = 0,
        .stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
    };

    VkSemaphoreSubmitInfo renderingSemaphoreSignalInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = state->renderingSemaphores[image_index],
        .deviceIndex = 0,
        .stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
    };

    VkSubmitInfo2 submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = &commandBufferSubmitInfo,
        .waitSemaphoreInfoCount = 1,
        .pWaitSemaphoreInfos = &presentSemaphoreWaitInfo,
        .signalSemaphoreInfoCount = 1,
        .pSignalSemaphoreInfos = &renderingSemaphoreSignalInfo,
    };

    vkQueueSubmit2(state->submitQueue, 1, &submitInfo, state->drawFences[frame_idx]);

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &(state->renderingSemaphores[image_index]),
        .swapchainCount = 1,
        .pSwapchains = &(state->swapchain),
        .pImageIndices = &image_index,
    };

    vkQueuePresentKHR(state->submitQueue, &presentInfo);
    state->frame_idx = (state->frame_idx + 1) % MAX_INFLIGHT_FRAMES;
}


// BASIC RENDERER IMPLEMENTATION BELOW

void _create_presentation_resources(Renderer const* renderer)
{
    LOG_INFO("Started creation of presentation resources\n");
    RendererState* state = (RendererState *)(renderer + 1);
    VkDevice device = renderer->core.device;
    VkPhysicalDevice physicalDevice = renderer->core.physical_device;
    VkSurfaceKHR surface = renderer->core.surface;

    VkSurfaceCapabilities2KHR surfaceCapabilities = {
        .sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR
    };
    VkPhysicalDeviceSurfaceInfo2KHR surfaceInfo = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR,
        .surface = surface
    };

    VkResult result = vkGetPhysicalDeviceSurfaceCapabilities2KHR(physicalDevice, &surfaceInfo, &surfaceCapabilities);
    VK_RESULT_CHECK(result);
    LOG_INFO("Obtained Physical Device Surface Capabilties\n");

    uint32_t formats_count = 0;
    result = vkGetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, &surfaceInfo, &formats_count, NULL);
    VkSurfaceFormat2KHR *formats = calloc(formats_count, sizeof *formats);
    for (uint32_t i = 0; i < formats_count; ++i) formats[i].sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
    result = vkGetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, &surfaceInfo, &formats_count, formats);
    VK_RESULT_CHECK(result);
    LOG_INFO("Obtained Supported Surface Formats\n");

    uint32_t present_modes_count = 0;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &present_modes_count, NULL);
    VkPresentModeKHR *presentModes = calloc(present_modes_count, sizeof *presentModes);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &present_modes_count, presentModes);
    VK_RESULT_CHECK(result);
    LOG_INFO("Obtained Supported Surface Present Modes\n");

    uint32_t max_image_count = surfaceCapabilities.surfaceCapabilities.maxImageCount;
    max_image_count = MAX_INFLIGHT_FRAMES > max_image_count ? max_image_count : MAX_INFLIGHT_FRAMES;
    VkExtent2D maxExtent = surfaceCapabilities.surfaceCapabilities.maxImageExtent;
    VkExtent2D minExtent = surfaceCapabilities.surfaceCapabilities.minImageExtent;
    VkExtent2D currentExtent = surfaceCapabilities.surfaceCapabilities.currentExtent;
    LOG_INFOF("\tMax Image Count: %d (real maximum: %d)\n", max_image_count, surfaceCapabilities.surfaceCapabilities.maxImageCount);
    LOG_INFOF("\tMax Extent width: %u height: %u\n", maxExtent.width, maxExtent.height);
    LOG_INFOF("\tMin Extent width: %u height: %u\n", minExtent.width, minExtent.height);
    LOG_INFOF("\tCurrent Extent width: %u height: %u\n", currentExtent.width, currentExtent.height);


    uint32_t frame_width, frame_height;
    get_window_framebuffer_size(renderer->window, &frame_width, &frame_height);
    // Clamp the frame extent
    frame_width = frame_width > maxExtent.width ? maxExtent.width : frame_width;
    frame_height = frame_height > maxExtent.height? maxExtent.height : frame_height;
    frame_width = frame_width < minExtent.width ? minExtent.width : frame_width;
    frame_height = frame_height < minExtent.height? minExtent.height : frame_height;
    LOG_INFOF("\tFrame width: %u height: %u\n", frame_width, frame_height);

    VkExtent2D frameExtent = { frame_width, frame_height };
    state->swapchainExtent = frameExtent;

    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB; // preferred
    VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;                                              
    VkSurfaceFormat2KHR *psurface_format;
    for (psurface_format = formats; psurface_format != (formats + formats_count); ++psurface_format)
    {
        if (psurface_format->surfaceFormat.format == format) {
            colorSpace = psurface_format->surfaceFormat.colorSpace;
            break;
        }
    }

    if (psurface_format == (formats + formats_count)) 
    {
        LOG_INFO("Preferrred format not found!\n");
        format = formats[0].surfaceFormat.format;
    }

    VkPresentModeKHR presentMode = presentModes[0];
    VkCompositeAlphaFlagsKHR compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;

    state->swapchainFormat = format;
    state->swapchainPresentMode = presentMode;

    VkSwapchainCreateInfoKHR swapchainInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .presentMode = presentMode,
        .imageFormat = format,
        .minImageCount = max_image_count,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = surfaceCapabilities.surfaceCapabilities.currentTransform,
        .compositeAlpha = compositeAlpha,
        .clipped = VK_TRUE,
        .imageExtent = frameExtent,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &renderer->core.graphics_family_idx,
    };

    result = vkCreateSwapchainKHR(device, &swapchainInfo, NULL, &state->swapchain);
    VK_RESULT_CHECK(result);

    if (state->swapchain == NULL)
        LOG_INFO("Failed to create swapchain\n");
    else 
        LOG_INFO("Successsfully created swapchain\n");

    LOG_INFO("Finished creating presentation resources\n");
    free(formats); formats = NULL;
    free(presentModes); presentModes = NULL; 
}

void _create_images_and_frame_buffers(Renderer const* renderer)
{
    RendererState* state = (RendererState *)(renderer + 1);
    VkDevice device = renderer->core.device;
    VkSwapchainKHR swapchain = state->swapchain;
    uint32_t swapchainImagesCount = 0;

    (vkGetSwapchainImagesKHR(device, swapchain, &swapchainImagesCount, NULL));
    state->swapchainImagesCount = swapchainImagesCount;
    state->swapchainImages = calloc(swapchainImagesCount, sizeof *(state->swapchainImages));
    state->swapchainImageViews = calloc(swapchainImagesCount, sizeof *(state->swapchainImageViews));
    (vkGetSwapchainImagesKHR(device, swapchain, &swapchainImagesCount, state->swapchainImages));
    LOG_INFO("Obtained swapchain images\n");
    
    VkImageViewCreateInfo imageviewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .format = state->swapchainFormat,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .subresourceRange = {
            .aspectMask  = VK_IMAGE_ASPECT_COLOR_BIT,
            .layerCount = 1,
            .baseArrayLayer = 0,
            .levelCount = 1,
            .baseMipLevel = 0,
        },
        .components = {
            .r = VK_COMPONENT_SWIZZLE_R,
            .g = VK_COMPONENT_SWIZZLE_G,
            .b = VK_COMPONENT_SWIZZLE_B,
            .a = VK_COMPONENT_SWIZZLE_A,
        }
    };

    for (uint32_t i = 0; i < swapchainImagesCount; ++i)
    {
        imageviewInfo.image = state->swapchainImages[i];
        vkCreateImageView(device, &imageviewInfo, NULL, &(state->swapchainImageViews[i]));
    }
    LOG_INFO("Created image views for the swapchain images\n");

    LOG_INFO("Finished creating images and framebuffers\n");
}

[[nodiscard]] VkShaderModule create_shader_module_from_file(VkDevice device, const char* contents, size_t size_bytes)
{
    VkShaderModule module;
    VkShaderModuleCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = size_bytes,
        .pCode = (const uint32_t *)contents
    };

    (vkCreateShaderModule(device, &createInfo, NULL, &module));

    return module;
}

char* read_file_contents(const char* path, uint32_t *bytes)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) return NULL;
    fseek(file, 0, SEEK_END);
    *bytes = ftell(file);
    char* contents = malloc(*bytes);
    fseek(file, 0, SEEK_SET);
    uint32_t ret = fread(contents, *bytes, *bytes, file);

    return contents;
}

void _create_graphics_pipeline(Renderer const* renderer)
{
    RendererState* state = (RendererState *)(renderer + 1);
    VkDevice device = renderer->core.device;
    VkSwapchainKHR swapchain = state->swapchain;
    
    uint32_t frag_file_bytes = 0, vert_file_bytes = 0;
    char* frag_spv = read_file_contents("shaders/basic.frag.spv", &frag_file_bytes);
    char* vert_spv = read_file_contents("shaders/basic.vert.spv", &vert_file_bytes);
    if (frag_spv == NULL)
    {
        LOG_INFO("Path to frag spirv binary was incorrect or failed to read\n");
        return;
    }

    if (vert_spv == NULL)
    {
        LOG_INFO("Path to Vertex spirv binary was incorrect or failed to read\n");
        return;
    }

    VkShaderModule fragModule = create_shader_module_from_file(device, frag_spv, frag_file_bytes);
    VkShaderModule vertModule = create_shader_module_from_file(device, vert_spv, vert_file_bytes);

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragModule,
        .pName = "main",
        .pSpecializationInfo = NULL,
    };

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertModule,
        .pName = "main",
        .pSpecializationInfo = NULL,
    };

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
    uint32_t shaderStagesCount = sizeof(shaderStages) / sizeof(shaderStages[0]);

    VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    uint32_t dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = { 
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = dynamicStateCount,
        .pDynamicStates = dynamicStates,
    };

    VkVertexInputAttributeDescription positionInputDesc = {
        .location = 0,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = 0
    };

    VkVertexInputAttributeDescription colorInputDesc = {
        .location = 1,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = sizeof(float) * 3
    };

    VkVertexInputAttributeDescription descs[] = { positionInputDesc, colorInputDesc };
    uint32_t vertex_input_attr_desc_count = sizeof(descs) / sizeof(descs[0]);

    VkVertexInputBindingDescription binding_desc = {
        .binding = 0,
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        .stride = sizeof(float) * (4 + 3),
    };

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pVertexAttributeDescriptions = descs,
        .vertexAttributeDescriptionCount = vertex_input_attr_desc_count,
        .pVertexBindingDescriptions = &binding_desc,
        .vertexBindingDescriptionCount = 1
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .primitiveRestartEnable = VK_FALSE,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    };

    VkViewport viewport = { 0.0f, 0.0f, (float)(state->swapchainExtent.width), (float)(state->swapchainExtent.height), 0.0f, 1.0f };
    VkRect2D scissor = { { 0, 0 }, state->swapchainExtent };

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
    };

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthBiasEnable = VK_FALSE,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .lineWidth = 1.0f,
    };

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .pAttachments = &colorBlendAttachmentState,
        .attachmentCount = 1,
    };

    VkPipelineLayout layout;
    VkPipelineLayoutCreateInfo layoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pushConstantRangeCount = 0,
    };
    (vkCreatePipelineLayout(device, &layoutCreateInfo, NULL, &layout));

    VkPipelineRenderingCreateInfo renderingCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &(state->swapchainFormat),
    };

    VkGraphicsPipelineCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .layout = layout,
        .pColorBlendState = &colorBlendStateCreateInfo,
        .pDepthStencilState = NULL,
        .pDynamicState = &dynamicStateCreateInfo,
        .pInputAssemblyState = &inputAssemblyStateCreateInfo,
        .pVertexInputState = &vertexInputStateCreateInfo,
        .pMultisampleState = &multisampleStateCreateInfo,
        .pRasterizationState = &rasterizationStateCreateInfo,
        .pTessellationState = NULL,
        .pViewportState = &viewportStateCreateInfo,
        .pStages = shaderStages,
        .stageCount = shaderStagesCount,
        .renderPass = VK_NULL_HANDLE,
        .pNext = &renderingCreateInfo
    };

    VkPipeline pipeline;
    (vkCreateGraphicsPipelines(device, NULL, 1, &createInfo, NULL, &pipeline));
    
    state->graphicsPipeline = pipeline;
    LOG_INFO("Created graphics pipeline\n");

    free(frag_spv); frag_spv = NULL;
    free(vert_spv); vert_spv = NULL;
}

void _create_command_pool_and_buffers(Renderer const* renderer)
{
    RendererState* state = (RendererState *)(renderer + 1);
    VkDevice device = renderer->core.device;

    VkCommandPoolCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = renderer->core.graphics_family_idx,
    };
    VkResult result = vkCreateCommandPool(device, &info, NULL, &state->commandPool);
    VK_RESULT_CHECK(result);
    
    VkCommandBufferAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = state->commandPool,
        .commandBufferCount = MAX_INFLIGHT_FRAMES,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    };

    result = (vkAllocateCommandBuffers(device, &allocateInfo, state->primaryCommandBuffers));
    VK_RESULT_CHECK(result);
}

void _create_sync_objects(Renderer const* renderer)
{
    RendererState* state = (RendererState *)(renderer + 1);
    VkDevice device = renderer->core.device;

    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    state->renderingSemaphores = calloc(state->swapchainImagesCount, sizeof *state->renderingSemaphores);

    for (uint32_t i = 0; i < MAX_INFLIGHT_FRAMES; ++i)
    {
        vkCreateFence(device, &fenceInfo, NULL, state->drawFences + i);
        vkCreateSemaphore(device, &semaphoreInfo, NULL, state->presentSemaphores + i);
    }

    for (uint32_t i = 0; i < state->swapchainImagesCount; ++i)
    {
        vkCreateSemaphore(device, &semaphoreInfo, NULL, state->renderingSemaphores + i);
    }

    LOG_INFO("Finished creating Sync objects\n");
}

void _create_queues(Renderer const* renderer)
{
    RendererState* state = (RendererState *)(renderer + 1);
    VkDevice device = renderer->core.device;
    uint32_t graphics_family_idx = renderer->core.graphics_family_idx;
    (vkGetDeviceQueue(device, graphics_family_idx, 0, &(state->submitQueue)));
}

void _create_basic_renderer(Renderer const* renderer)
{
    // create shaders
    // create graphics pipeline with dynamic state
    // create command pool and command buffers
    // create image and imageviews
   
    _create_presentation_resources(renderer);
    _create_images_and_frame_buffers(renderer);
    _create_graphics_pipeline(renderer);
    _create_command_pool_and_buffers(renderer);
    _create_sync_objects(renderer);
    _create_queues(renderer);
}
