#include "vulkan_internal.hpp"
#include <SDL3/SDL_vulkan.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
namespace tr::vk {
bool init(Renderer *r) {
    auto *state = new Context;
    r->gpu = state;
    auto &g = *state;
    g.screen.owner = r;
    const bool headless = std::strcmp(SDL_GetCurrentVideoDriver(), "dummy") == 0;
    VkApplicationInfo app{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app.pApplicationName = "Teeming";
    app.apiVersion = VK_API_VERSION_1_0;
    VkInstanceCreateInfo instance{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    instance.pApplicationInfo = &app;
    Uint32 count = 0;
    if (!headless) {
        instance.ppEnabledExtensionNames = SDL_Vulkan_GetInstanceExtensions(&count);
        if (!instance.ppEnabledExtensionNames)
            return false;
        instance.enabledExtensionCount = count;
    }
    const char *validation = "VK_LAYER_KHRONOS_validation";
    if (SDL_getenv("TEEMING_VK_VALIDATION")) {
        instance.enabledLayerCount = 1;
        instance.ppEnabledLayerNames = &validation;
    }
    if (!check(vkCreateInstance(&instance, nullptr, &g.instance), "create instance"))
        return false;
    if (!headless && !SDL_Vulkan_CreateSurface(r->window, g.instance, nullptr, &g.surface))
        return false;
    std::uint32_t n = 0;
    vkEnumeratePhysicalDevices(g.instance, &n, nullptr);
    std::vector<VkPhysicalDevice> devices(n);
    vkEnumeratePhysicalDevices(g.instance, &n, devices.data());
    int score = -1;
    for (auto device : devices) {
        std::uint32_t qn = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &qn, nullptr);
        std::vector<VkQueueFamilyProperties> queues(qn);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &qn, queues.data());
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);
        for (std::uint32_t q = 0; q < qn; ++q) {
            VkBool32 supported = VK_TRUE;
            if (g.surface)
                vkGetPhysicalDeviceSurfaceSupportKHR(device, q, g.surface, &supported);
            if (!supported || !(queues[q].queueFlags & VK_QUEUE_GRAPHICS_BIT))
                continue;
            const int rank = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU     ? 3
                             : properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? 2
                                                                                               : 1;
            if (rank > score) {
                g.physical = device;
                g.family = q;
                score = rank;
            }
        }
    }
    if (!g.physical)
        return SDL_SetError("No compatible Vulkan graphics device");
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(g.physical, &properties);
    std::printf("Vulkan device: %s%s\n", properties.deviceName, headless ? " (offscreen)" : "");
    float priority = 1;
    VkDeviceQueueCreateInfo queue{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    queue.queueFamilyIndex = g.family;
    queue.queueCount = 1;
    queue.pQueuePriorities = &priority;
    VkDeviceCreateInfo device{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    device.queueCreateInfoCount = 1;
    device.pQueueCreateInfos = &queue;
    const char *extension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    if (g.surface) {
        device.enabledExtensionCount = 1;
        device.ppEnabledExtensionNames = &extension;
    }
    if (!check(vkCreateDevice(g.physical, &device, nullptr, &g.device), "create device"))
        return false;
    vkGetDeviceQueue(g.device, g.family, 0, &g.queue);
    VkCommandPoolCreateInfo pool{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    pool.queueFamilyIndex = g.family;
    pool.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    if (!check(vkCreateCommandPool(g.device, &pool, nullptr, &g.pool), "command pool"))
        return false;
    VkCommandBufferAllocateInfo command{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    command.commandPool = g.pool;
    command.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command.commandBufferCount = 1;
    if (!check(vkAllocateCommandBuffers(g.device, &command, &g.command), "command buffer"))
        return false;
    VkFenceCreateInfo fence{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    if (!check(vkCreateFence(g.device, &fence, nullptr, &g.fence), "fence"))
        return false;
    VkSemaphoreCreateInfo semaphore{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    if (g.surface &&
        !check(vkCreateSemaphore(g.device, &semaphore, nullptr, &g.acquired), "acquire semaphore"))
        return false;
    return init_pipeline(g) && resize_screen(r);
}
void shutdown(Renderer *r) {
    if (!r->gpu)
        return;
    auto &g = ctx(r);
    if (g.device) {
        vkDeviceWaitIdle(g.device);
        g.pending = false;
        destroy(&g.screen);
        destroy_buffer(g, g.vertices);
        destroy_buffer(g, g.indices);
        for (auto [key, p] : g.pipelines)
            vkDestroyPipeline(g.device, p, nullptr);
        for (auto s : g.finished)
            vkDestroySemaphore(g.device, s, nullptr);
        if (g.acquired)
            vkDestroySemaphore(g.device, g.acquired, nullptr);
        if (g.swapchain)
            vkDestroySwapchainKHR(g.device, g.swapchain, nullptr);
        if (g.linear)
            vkDestroySampler(g.device, g.linear, nullptr);
        if (g.nearest)
            vkDestroySampler(g.device, g.nearest, nullptr);
        if (g.descriptors)
            vkDestroyDescriptorPool(g.device, g.descriptors, nullptr);
        if (g.pipeline_layout)
            vkDestroyPipelineLayout(g.device, g.pipeline_layout, nullptr);
        if (g.set_layout)
            vkDestroyDescriptorSetLayout(g.device, g.set_layout, nullptr);
        if (g.pass)
            vkDestroyRenderPass(g.device, g.pass, nullptr);
        if (g.fence)
            vkDestroyFence(g.device, g.fence, nullptr);
        if (g.pool)
            vkDestroyCommandPool(g.device, g.pool, nullptr);
        vkDestroyDevice(g.device, nullptr);
    }
    if (g.surface)
        vkDestroySurfaceKHR(g.instance, g.surface, nullptr);
    if (g.instance)
        vkDestroyInstance(g.instance, nullptr);
    delete static_cast<Context *>(r->gpu);
    r->gpu = nullptr;
}
static bool swapchain(Renderer *r) {
    auto &g = ctx(r);
    VkSurfaceCapabilitiesKHR cap;
    if (!check(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(g.physical, g.surface, &cap),
               "surface capabilities"))
        return false;
    if (!cap.currentExtent.width || !cap.currentExtent.height)
        return false;
    VkExtent2D extent = cap.currentExtent;
    if (extent.width == UINT32_MAX) {
        int w, h;
        SDL_GetWindowSizeInPixels(r->window, &w, &h);
        extent = {std::clamp(static_cast<std::uint32_t>(std::max(w, 1)), cap.minImageExtent.width,
                             cap.maxImageExtent.width),
                  std::clamp(static_cast<std::uint32_t>(std::max(h, 1)), cap.minImageExtent.height,
                             cap.maxImageExtent.height)};
    }
    if (g.swapchain && g.extent.width == extent.width && g.extent.height == extent.height &&
        g.swap_vsync == r->vsync)
        return true;
    vkDeviceWaitIdle(g.device);
    g.pending = false;
    std::uint32_t n = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(g.physical, g.surface, &n, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(n);
    vkGetPhysicalDeviceSurfaceFormatsKHR(g.physical, g.surface, &n, formats.data());
    if (formats.empty())
        return SDL_SetError("No Vulkan surface formats");
    auto format = formats.front();
    for (auto f : formats)
        if (f.format == VK_FORMAT_B8G8R8A8_UNORM || f.format == VK_FORMAT_R8G8B8A8_UNORM) {
            format = f;
            break;
        }
    if (!(cap.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
        return SDL_SetError("Vulkan surface cannot receive image copies");
    VkPresentModeKHR mode = VK_PRESENT_MODE_FIFO_KHR;
    if (!r->vsync) {
        vkGetPhysicalDeviceSurfacePresentModesKHR(g.physical, g.surface, &n, nullptr);
        std::vector<VkPresentModeKHR> modes(n);
        vkGetPhysicalDeviceSurfacePresentModesKHR(g.physical, g.surface, &n, modes.data());
        for (auto m : modes)
            if (m == VK_PRESENT_MODE_IMMEDIATE_KHR)
                mode = m;
    }
    VkSwapchainCreateInfoKHR info{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    info.surface = g.surface;
    info.minImageCount = cap.minImageCount + 1;
    if (cap.maxImageCount)
        info.minImageCount = std::min(info.minImageCount, cap.maxImageCount);
    info.imageFormat = format.format;
    info.imageColorSpace = format.colorSpace;
    info.imageExtent = extent;
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.preTransform = cap.currentTransform;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    for (auto a : {VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
                   VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR})
        if (cap.supportedCompositeAlpha & a) {
            info.compositeAlpha = a;
            break;
        }
    info.presentMode = mode;
    info.clipped = VK_TRUE;
    info.oldSwapchain = g.swapchain;
    VkSwapchainKHR next;
    if (!check(vkCreateSwapchainKHR(g.device, &info, nullptr, &next), "swapchain"))
        return false;
    for (auto s : g.finished)
        vkDestroySemaphore(g.device, s, nullptr);
    if (g.swapchain)
        vkDestroySwapchainKHR(g.device, g.swapchain, nullptr);
    g.swapchain = next;
    g.extent = extent;
    g.swap_format = format.format;
    g.swap_vsync = r->vsync;
    vkGetSwapchainImagesKHR(g.device, g.swapchain, &n, nullptr);
    g.swap_images.resize(n);
    vkGetSwapchainImagesKHR(g.device, g.swapchain, &n, g.swap_images.data());
    g.finished.assign(n, VK_NULL_HANDLE);
    VkSemaphoreCreateInfo sem{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    for (auto &s : g.finished)
        if (!check(vkCreateSemaphore(g.device, &sem, nullptr, &s), "present semaphore"))
            return false;
    return true;
}
bool present(Renderer *r) {
    auto &g = ctx(r);
    if (!g.surface)
        return true;
    if (SDL_GetWindowFlags(r->window) & SDL_WINDOW_MINIMIZED)
        return true;
    if (!swapchain(r))
        return false;
    if (!wait(g))
        return false;
    std::uint32_t index = 0;
    VkResult result =
        vkAcquireNextImageKHR(g.device, g.swapchain, UINT64_MAX, g.acquired, VK_NULL_HANDLE, &index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        g.swap_vsync = -1;
        return true;
    }
    if (result != VK_SUBOPTIMAL_KHR && !check(result, "acquire image"))
        return false;
    if (!begin(g))
        return false;
    transition(g, img(&g.screen), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    Image dest;
    dest.image = g.swap_images[index];
    transition(g, dest, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    VkImageBlit blit{};
    blit.srcSubresource = blit.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    blit.srcOffsets[1] = {g.screen.w, g.screen.h, 1};
    blit.dstOffsets[1] = {static_cast<int>(g.extent.width), static_cast<int>(g.extent.height), 1};
    vkCmdBlitImage(g.command, img(&g.screen).image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dest.image,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_NEAREST);
    transition(g, dest, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    if (!finish(g, g.finished[index], g.acquired))
        return false;
    VkPresentInfoKHR present{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present.waitSemaphoreCount = 1;
    present.pWaitSemaphores = &g.finished[index];
    present.swapchainCount = 1;
    present.pSwapchains = &g.swapchain;
    present.pImageIndices = &index;
    result = vkQueuePresentKHR(g.queue, &present);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        g.swap_vsync = -1;
        return true;
    }
    return check(result, "present");
}
} // namespace tr::vk
namespace tr {
const Backend &vulkan_backend() {
    static const Backend api{"teeming-vulkan", vk::init,    vk::shutdown, vk::create, vk::destroy,
                             vk::upload,       vk::execute, vk::present,  vk::read};
    return api;
}
} // namespace tr
