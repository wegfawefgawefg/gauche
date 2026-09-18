#version 450
layout(binding=0) uniform sampler2D sprite;
layout(binding=1) uniform sampler2D lightmap;
layout(location=0) in vec4 vertex_color;
layout(location=1) in vec2 texture_uv;
layout(location=2) in vec2 lighting_uv;
layout(location=0) out vec4 output_color;
void main(){output_color=texture(sprite,texture_uv)*vertex_color*vec4(texture(lightmap,lighting_uv).rgb,1.0);}
