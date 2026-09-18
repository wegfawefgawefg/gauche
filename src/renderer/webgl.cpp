#include "device.hpp"
#include <GLES3/gl3.h>
#include <algorithm>
#include <cstddef>
#include <cstring>

namespace tr {
namespace {
struct Image {
    GLuint texture = 0, fbo = 0;
};
struct Context {
    SDL_GLContext context = nullptr;
    GLuint program = 0, vao = 0, vbo = 0, ibo = 0;
    GLint size = 0;
    GLuint samplers[2]{};
};
Context &ctx(Renderer *r) {
    return *static_cast<Context *>(r->gpu);
}
Image &img(Texture *t) {
    return *static_cast<Image *>(t->gpu);
}
GLuint shader(GLenum type, const char *source) {
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &source, nullptr);
    glCompileShader(sh);
    GLint ok = 0;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[4096];
        glGetShaderInfoLog(sh, sizeof(log), nullptr, log);
        SDL_SetError("Shader: %s", log);
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}
bool init(Renderer *r) {
    auto *g = new Context;
    r->gpu = g;
    g->context = SDL_GL_CreateContext(r->window);
    if (!g->context)
        return false;
    const char *vs = R"(#version 300 es
precision highp float;
layout(location=0) in vec3 position;layout(location=1) in vec4 color;layout(location=2) in vec2 uv;layout(location=3) in vec2 light_uv;
uniform vec3 size;out vec4 vertex_color;out vec2 texture_uv;out vec2 lighting_uv;
void main(){vec2 p=position.xy/size.xy*2.0-1.0;p.y*=size.z;gl_Position=vec4(p,position.z,1);vertex_color=color;texture_uv=uv;lighting_uv=light_uv;})";
    const char *fs = R"(#version 300 es
precision highp float;
uniform sampler2D sprite;uniform sampler2D lightmap;in vec4 vertex_color;in vec2 texture_uv;in vec2 lighting_uv;out vec4 output_color;
void main(){output_color=texture(sprite,texture_uv)*vertex_color*vec4(texture(lightmap,lighting_uv).rgb,1);})";
    GLuint v = shader(GL_VERTEX_SHADER, vs), f = shader(GL_FRAGMENT_SHADER, fs);
    if (!v || !f)
        return false;
    g->program = glCreateProgram();
    glAttachShader(g->program, v);
    glAttachShader(g->program, f);
    glLinkProgram(g->program);
    glDeleteShader(v);
    glDeleteShader(f);
    GLint ok;
    glGetProgramiv(g->program, GL_LINK_STATUS, &ok);
    if (!ok)
        return SDL_SetError("Sprite program link failed");
    glUseProgram(g->program);
    g->size = glGetUniformLocation(g->program, "size");
    glUniform1i(glGetUniformLocation(g->program, "sprite"), 0);
    glUniform1i(glGetUniformLocation(g->program, "lightmap"), 1);
    glGenVertexArrays(1, &g->vao);
    glBindVertexArray(g->vao);
    glGenBuffers(1, &g->vbo);
    glGenBuffers(1, &g->ibo);
    glBindBuffer(GL_ARRAY_BUFFER, g->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g->ibo);
    const int counts[]{3, 4, 2, 2};
    const std::size_t offsets[]{offsetof(Vertex, x), offsetof(Vertex, r), offsetof(Vertex, u),
                                offsetof(Vertex, lu)};
    for (GLuint i = 0; i < 4; ++i) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, counts[i], GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<void *>(offsets[i]));
    }
    glGenSamplers(2, g->samplers);
    for (int i = 0; i < 2; ++i) {
        glSamplerParameteri(g->samplers[i], GL_TEXTURE_MIN_FILTER, i ? GL_LINEAR : GL_NEAREST);
        glSamplerParameteri(g->samplers[i], GL_TEXTURE_MAG_FILTER, i ? GL_LINEAR : GL_NEAREST);
        glSamplerParameteri(g->samplers[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(g->samplers[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    return true;
}
void shutdown(Renderer *r) {
    if (!r->gpu)
        return;
    auto &g = ctx(r);
    if (g.context) {
        glDeleteSamplers(2, g.samplers);
        glDeleteBuffers(1, &g.vbo);
        glDeleteBuffers(1, &g.ibo);
        glDeleteVertexArrays(1, &g.vao);
        glDeleteProgram(g.program);
        SDL_GL_DestroyContext(g.context);
    }
    delete static_cast<Context *>(r->gpu);
    r->gpu = nullptr;
}
bool create(Texture *t) {
    auto *im = new Image;
    t->gpu = im;
    glGenTextures(1, &im->texture);
    glBindTexture(GL_TEXTURE_2D, im->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, t->w, t->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if (t->target) {
        glGenFramebuffers(1, &im->fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, im->fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, im->texture, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            return SDL_SetError("Incomplete render target");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    return glGetError() == GL_NO_ERROR;
}
void destroy(Texture *t) {
    auto *im = static_cast<Image *>(t->gpu);
    if (!im)
        return;
    glDeleteFramebuffers(1, &im->fbo);
    glDeleteTextures(1, &im->texture);
    delete im;
}
bool upload(Texture *t, const SDL_Rect &area, const void *data, int pitch) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, img(t).texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, pitch / 4);
    glTexSubImage2D(GL_TEXTURE_2D, 0, area.x, area.y, area.w, area.h, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    return glGetError() == GL_NO_ERROR;
}
void blend(SDL_BlendMode mode) {
    if (mode == SDL_BLENDMODE_NONE) {
        glDisable(GL_BLEND);
        return;
    }
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    if (mode == SDL_BLENDMODE_ADD || mode == SDL_BLENDMODE_ADD_PREMULTIPLIED)
        glBlendFuncSeparate(mode == SDL_BLENDMODE_ADD_PREMULTIPLIED ? GL_ONE : GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE);
    else if (mode == SDL_BLENDMODE_MOD)
        glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_ONE);
    else if (mode == SDL_BLENDMODE_MUL)
        glBlendFuncSeparate(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
    else
        glBlendFuncSeparate(mode == SDL_BLENDMODE_BLEND_PREMULTIPLIED ? GL_ONE : GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}
bool execute(Renderer *r) {
    auto &g = ctx(r);
    glUseProgram(g.program);
    glBindVertexArray(g.vao);
    glBindBuffer(GL_ARRAY_BUFFER, g.vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(r->vertices.size() * sizeof(Vertex)),
                 r->vertices.data(), GL_STREAM_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(r->indices.size() * sizeof(std::uint32_t)), r->indices.data(),
                 GL_STREAM_DRAW);
    for (const auto &c : r->commands) {
        int w, h;
        if (c.target) {
            w = c.target->w;
            h = c.target->h;
        } else
            SDL_GetWindowSizeInPixels(r->window, &w, &h);
        glBindFramebuffer(GL_FRAMEBUFFER, c.target ? img(c.target).fbo : 0);
        glViewport(0, 0, w, h);
        if (c.clear) {
            glDisable(GL_SCISSOR_TEST);
            glClearColor(c.color.r, c.color.g, c.color.b, c.color.a);
            glClear(GL_COLOR_BUFFER_BIT);
            continue;
        }
        glUniform3f(g.size, static_cast<float>(w), static_cast<float>(h), c.target ? 1.F : -1.F);
        glEnable(GL_SCISSOR_TEST);
        glScissor(c.clip.x, c.target ? c.clip.y : h - c.clip.y - c.clip.h, c.clip.w, c.clip.h);
        blend(c.blend);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, img(c.texture).texture);
        glBindSampler(0, g.samplers[c.filter == SDL_SCALEMODE_LINEAR ? 1 : 0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, img(c.light).texture);
        glBindSampler(1, g.samplers[1]);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(c.count), GL_UNSIGNED_INT,
                       reinterpret_cast<void *>(static_cast<std::uintptr_t>(c.first) * 4));
    }
    const auto error = glGetError();
    if (error)
        return SDL_SetError("WebGL rendering error %u", error);
    return true;
}
bool present_frame(Renderer *r) {
    return SDL_GL_SwapWindow(r->window);
}
SDL_Surface *read(Renderer *r, Texture *t, const SDL_Rect &area) {
    auto *s = SDL_CreateSurface(area.w, area.h, SDL_PIXELFORMAT_RGBA32);
    if (!s)
        return nullptr;
    int w, h;
    if (t) {
        w = t->w;
        h = t->h;
    } else
        SDL_GetWindowSizeInPixels(r->window, &w, &h);
    glBindFramebuffer(GL_FRAMEBUFFER, t ? img(t).fbo : 0);
    glReadPixels(area.x, t ? area.y : h - area.y - area.h, area.w, area.h, GL_RGBA, GL_UNSIGNED_BYTE,
                 s->pixels);
    if (!t) {
        std::vector<char> row(static_cast<std::size_t>(s->pitch));
        for (int y = 0; y < s->h / 2; ++y) {
            auto *a = static_cast<char *>(s->pixels) + y * s->pitch;
            auto *b = static_cast<char *>(s->pixels) + (s->h - 1 - y) * s->pitch;
            std::memcpy(row.data(), a, row.size());
            std::memcpy(a, b, row.size());
            std::memcpy(b, row.data(), row.size());
        }
    }
    return s;
}
} // namespace
const Backend &webgl_backend() {
    static const Backend api{"teeming-webgl2", init,    shutdown,      create, destroy,
                             upload,           execute, present_frame, read};
    return api;
}
} // namespace tr
