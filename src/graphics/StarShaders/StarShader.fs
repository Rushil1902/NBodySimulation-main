#version 330 core
out vec4 FragColor;

uniform vec3 starColor;

void main()
{
    FragColor = vec4(starColor, 1.0f);
}
