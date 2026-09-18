#pragma once
#include "device.hpp"
#include <map>
#include <tuple>
#include <vulkan/vulkan.h>
namespace tr::vk {
struct Buffer {
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceSize capacity = 0;
    void *mapped = nullptr;
};
struct Image {
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
};
struct Context {
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physical = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkQueue queue = VK_NULL_HANDLE;
    std::uint32_t family = 0;
    VkCommandPool pool = VK_NULL_HANDLE;
    VkCommandBuffer command = VK_NULL_HANDLE;
    VkFence fence = VK_NULL_HANDLE;
    VkRenderPass pass = VK_NULL_HANDLE;
    VkDescriptorSetLayout set_layout = VK_NULL_HANDLE;
    VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
    VkDescriptorPool descriptors = VK_NULL_HANDLE;
    VkSampler nearest = VK_NULL_HANDLE, linear = VK_NULL_HANDLE;
    std::map<SDL_BlendMode, VkPipeline> pipelines;
    std::map<std::tuple<Image *, Image *, int>, VkDescriptorSet> sets;
    Buffer vertices, indices;
    Texture screen;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkFormat swap_format = VK_FORMAT_UNDEFINED;
    VkExtent2D extent{};
    std::vector<VkImage> swap_images;
    std::vector<VkSemaphore> finished;
    VkSemaphore acquired = VK_NULL_HANDLE;
    int swap_vsync = -1;
    bool pending = false;
};
inline Context &ctx(Renderer *r) {
    return *static_cast<Context *>(r->gpu);
}
inline Image &img(Texture *t) {
    return *static_cast<Image *>(t->gpu);
}
bool check(VkResult, const char *);
std::uint32_t memory_type(Context &, std::uint32_t, VkMemoryPropertyFlags);
bool wait(Context &);
bool begin(Context &);
bool finish(Context &, VkSemaphore signal = VK_NULL_HANDLE, VkSemaphore waits = VK_NULL_HANDLE);
void destroy_buffer(Context &, Buffer &);
bool buffer(Context &, Buffer &, VkDeviceSize, VkBufferUsageFlags);
void transition(Context &, Image &, VkImageLayout);
bool create(Texture *);
void destroy(Texture *);
bool upload(Texture *, const SDL_Rect &, const void *, int);
bool init(Renderer *);
void shutdown(Renderer *);
bool init_pipeline(Context &);
VkPipeline pipeline(Context &, SDL_BlendMode);
VkDescriptorSet descriptors(Context &, Texture *, Texture *, SDL_ScaleMode);
bool execute(Renderer *);
bool present(Renderer *);
SDL_Surface *read(Renderer *, Texture *, const SDL_Rect &);
bool resize_screen(Renderer *);
} // namespace tr::vk
