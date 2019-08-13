#version 430 core

out vec4 color;

in VS_OUT
{
    vec2 tc;
} fs_in;

layout(binding=0) uniform sampler2D tex;

void main(void)
{
    color = vec4( texture(tex, fs_in.tc).xyz, 0.9);
}
