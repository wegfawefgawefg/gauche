#version 450
layout(location=0) in vec3 position;
layout(location=1) in vec4 color;
layout(location=2) in vec2 uv;
layout(location=3) in vec2 light_uv;
layout(push_constant) uniform Frame { vec4 size; } frame;
layout(location=0) out vec4 vertex_color;
layout(location=1) out vec2 texture_uv;
layout(location=2) out vec2 lighting_uv;
void main(){
    gl_Position=vec4(position.xy/frame.size.xy*2.0-1.0,position.z,1.0);
    vertex_color=color;texture_uv=uv;lighting_uv=light_uv;
}
