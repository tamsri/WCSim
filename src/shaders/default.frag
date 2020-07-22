#version 330 core
out vec4 FragColor;

in vec2 texture_coord;

 
uniform sampler2D input_texture;

void main()
{
    FragColor = texture(input_texture, texture_coord);
}