#version 330 core
layout (location = 0) in vec3 input_vertex_position;
layout (location = 1) in vec2 input_texture_coord;

out vec4 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 input_color;

void main()
{
    gl_Position = projection * view * model * vec4(input_vertex_position, 1.0);
    color = input_color;
}