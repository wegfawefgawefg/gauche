#include "vulkan_internal.hpp"
#include <cstring>
namespace tr::vk {
bool execute(Renderer *r) {
    auto &g = ctx(r);
    if (!resize_screen(r) || !begin(g))
        return false;
    if (!r->vertices.empty()) {
        if (!buffer(g, g.vertices, r->vertices.size() * sizeof(Vertex),
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) ||
            !buffer(g, g.indices, r->indices.size() * sizeof(std::uint32_t),
                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT))
            return false;
        std::memcpy(g.vertices.mapped, r->vertices.data(), r->vertices.size() * sizeof(Vertex));
        std::memcpy(g.indices.mapped, r->indices.data(), r->indices.size() * sizeof(std::uint32_t));
    }
    Texture *active = nullptr;
    for (const auto &c : r->commands) {
        auto *target = c.target ? c.target : &g.screen;
        // Sampling a previously rendered target needs a barrier outside the pass.
        const bool barrier =
            !c.clear && (img(c.texture).layout != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ||
                         img(c.light).layout != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        if (active && (active != target || barrier)) {
            vkCmdEndRenderPass(g.command);
            active = nullptr;
        }
        if (!active) {
            if (!c.clear) {
                transition(g, img(c.texture), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                transition(g, img(c.light), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            }
            transition(g, img(target), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
            VkRenderPassBeginInfo pass{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
            pass.renderPass = g.pass;
            pass.framebuffer = img(target).framebuffer;
            pass.renderArea.extent = {static_cast<std::uint32_t>(target->w),
                                      static_cast<std::uint32_t>(target->h)};
            vkCmdBeginRenderPass(g.command, &pass, VK_SUBPASS_CONTENTS_INLINE);
            active = target;
            VkViewport viewport{0, 0, static_cast<float>(target->w), static_cast<float>(target->h),
                                0, 1};
            vkCmdSetViewport(g.command, 0, 1, &viewport);
            float size[]{static_cast<float>(target->w), static_cast<float>(target->h), 0, 0};
            vkCmdPushConstants(g.command, g.pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(size),
                               size);
        }
        if (c.clear) {
            VkClearAttachment clear{};
            clear.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            clear.colorAttachment = 0;
            clear.clearValue.color = {{c.color.r, c.color.g, c.color.b, c.color.a}};
            VkClearRect rect{
                {{0, 0}, {static_cast<std::uint32_t>(target->w), static_cast<std::uint32_t>(target->h)}},
                0,
                1};
            vkCmdClearAttachments(g.command, 1, &clear, 1, &rect);
            continue;
        }
        const auto p = pipeline(g, c.blend);
        const auto set = descriptors(g, c.texture, c.light, c.filter);
        if (!p || !set)
            return false;
        vkCmdBindPipeline(g.command, VK_PIPELINE_BIND_POINT_GRAPHICS, p);
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(g.command, 0, 1, &g.vertices.buffer, &offset);
        vkCmdBindIndexBuffer(g.command, g.indices.buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(g.command, VK_PIPELINE_BIND_POINT_GRAPHICS, g.pipeline_layout, 0, 1,
                                &set, 0, nullptr);
        VkRect2D clip{{c.clip.x, c.clip.y},
                      {static_cast<std::uint32_t>(c.clip.w), static_cast<std::uint32_t>(c.clip.h)}};
        vkCmdSetScissor(g.command, 0, 1, &clip);
        vkCmdDrawIndexed(g.command, c.count, 1, c.first, 0, 0);
    }
    if (active)
        vkCmdEndRenderPass(g.command);
    return finish(g);
}
SDL_Surface *read(Renderer *r, Texture *texture, const SDL_Rect &area) {
    auto &g = ctx(r);
    auto *t = texture ? texture : &g.screen;
    if (area.x < 0 || area.y < 0 || area.w <= 0 || area.h <= 0 || area.x + area.w > t->w ||
        area.y + area.h > t->h) {
        SDL_SetError("Readback outside target");
        return nullptr;
    }
    if (!begin(g))
        return nullptr;
    Buffer staging;
    if (!buffer(g, staging, static_cast<VkDeviceSize>(area.w) * area.h * 4,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT))
        return nullptr;
    transition(g, img(t), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    VkBufferImageCopy copy{};
    copy.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    copy.imageOffset = {area.x, area.y, 0};
    copy.imageExtent = {static_cast<std::uint32_t>(area.w), static_cast<std::uint32_t>(area.h), 1};
    vkCmdCopyImageToBuffer(g.command, img(t).image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, staging.buffer,
                           1, &copy);
    SDL_Surface *result = nullptr;
    if (finish(g) && wait(g)) {
        result = SDL_CreateSurface(area.w, area.h, SDL_PIXELFORMAT_RGBA32);
        if (result)
            for (int y = 0; y < area.h; ++y)
                std::memcpy(static_cast<char *>(result->pixels) + y * result->pitch,
                            static_cast<char *>(staging.mapped) + y * area.w * 4,
                            static_cast<std::size_t>(area.w) * 4);
    }
    destroy_buffer(g, staging);
    return result;
}
} // namespace tr::vk
