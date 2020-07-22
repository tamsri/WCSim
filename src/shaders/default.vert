#version 330 core
layout (location = 0) in vec3 input_vertex_position;
layout (location = 1) in vec2 input_texture_coord;

out vec2 texture_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(input_vertex_position, 1.0);
    texture_coord = input_texture_coord;
}