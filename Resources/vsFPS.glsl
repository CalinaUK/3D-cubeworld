#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 tc;

out VS_OUT
{
    vec2 tc;
} vs_out;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

void main(void)
{
    gl_Position = proj_matrix * mv_matrix * position;
    vs_out.tc = tc;
}
