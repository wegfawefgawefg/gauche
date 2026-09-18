#include "device.hpp"
#include <array>
#include <emscripten.h>
namespace tr {
EM_ASYNC_JS(int, probe_webgpu, (), { return await Module.initOwnedWebGPU() ? 1 : 0; });
bool webgpu_available() {
    return probe_webgpu() != 0;
}
namespace {
EM_JS(int, create_image, (int w, int h), { return Module.ownedGPU.create(w, h); });
EM_JS(void, delete_image, (int id), { Module.ownedGPU.destroy(id); });
EM_JS(void, upload_image, (int id, int x, int y, int w, int h, const void *data, int pitch),
      { Module.ownedGPU.upload(id, x, y, w, h, data, pitch); });
EM_JS(int, execute_commands,
      (const void *vertices, int vb, const void *indices, int ib, const void *commands, int count, int w,
       int h),
      { return Module.ownedGPU.execute(vertices, vb, indices, ib, commands, count, w, h) ? 1 : 0; });
EM_JS(int, present_gpu, (), { return Module.ownedGPU.present() ? 1 : 0; });
EM_JS(void, shutdown_gpu, (), { Module.ownedGPU ?.shutdown(); });
EM_ASYNC_JS(int, read_gpu, (int id, int x, int y, int w, int h, void *output), {
    try {
        const bytes = await Module.ownedGPU.read(id, x, y, w, h);
        HEAPU8.set(bytes, output);
        return 1;
    } catch (e) {
        Module.printErr(String(e));
        return 0;
    }
});
int id(Texture *t) {
    return t ? static_cast<int>(reinterpret_cast<std::uintptr_t>(t->gpu)) : 0;
}
bool init(Renderer *) {
    return true;
}
void shutdown(Renderer *) {
    shutdown_gpu();
}
bool create(Texture *t) {
    const int value = create_image(t->w, t->h);
    t->gpu = reinterpret_cast<void *>(static_cast<std::uintptr_t>(value));
    return value != 0;
}
void destroy(Texture *t) {
    delete_image(id(t));
}
bool upload(Texture *t, const SDL_Rect &a, const void *pixels, int pitch) {
    upload_image(id(t), a.x, a.y, a.w, a.h, pixels, pitch);
    return true;
}
struct WireCommand {
    std::uint32_t target, texture, light, first, count;
    std::int32_t x, y, w, h;
    std::uint32_t blend, filter, clear;
    float color[4];
};
static_assert(sizeof(WireCommand) == 64);
bool execute(Renderer *r) {
    std::vector<WireCommand> commands;
    commands.reserve(r->commands.size());
    for (const auto &c : r->commands)
        commands.push_back({static_cast<std::uint32_t>(id(c.target)),
                            static_cast<std::uint32_t>(id(c.texture)),
                            static_cast<std::uint32_t>(id(c.light)),
                            c.first,
                            c.count,
                            c.clip.x,
                            c.clip.y,
                            c.clip.w,
                            c.clip.h,
                            c.blend,
                            c.filter == SDL_SCALEMODE_LINEAR ? 1U : 0U,
                            c.clear ? 1U : 0U,
                            {c.color.r, c.color.g, c.color.b, c.color.a}});
    int w, h;
    SDL_GetWindowSizeInPixels(r->window, &w, &h);
    if (!execute_commands(r->vertices.data(), static_cast<int>(r->vertices.size() * sizeof(Vertex)),
                          r->indices.data(), static_cast<int>(r->indices.size() * 4), commands.data(),
                          static_cast<int>(commands.size()), w, h))
        return SDL_SetError("WebGPU device is lost");
    return true;
}
bool present_frame(Renderer *) {
    return present_gpu() != 0 || SDL_SetError("WebGPU presentation failed");
}
SDL_Surface *read(Renderer *, Texture *t, const SDL_Rect &a) {
    auto *s = SDL_CreateSurface(a.w, a.h, SDL_PIXELFORMAT_RGBA32);
    if (!s)
        return nullptr;
    if (!read_gpu(id(t), a.x, a.y, a.w, a.h, s->pixels)) {
        SDL_DestroySurface(s);
        return nullptr;
    }
    return s;
}
} // namespace
const Backend &webgpu_backend() {
    static const Backend api{"teeming-webgpu", init,    shutdown,      create, destroy,
                             upload,           execute, present_frame, read};
    return api;
}
} // namespace tr
