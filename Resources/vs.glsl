#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 tc;
layout (location = 2) in vec4 normals;

out VS_OUT
{
    vec2 tc;
    vec4 normals;
    vec4 fragPos;
} vs_out;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

void main(void)
{
    gl_Position = proj_matrix * view_matrix * model_matrix * position;
    vs_out.tc = tc;

    vec3 normalsT = mat3(transpose(inverse(model_matrix))) * vec3(normals.xyz);
    vs_out.normals = vec4(normalsT, 1.0);

    vs_out.fragPos = model_matrix * position;
}
