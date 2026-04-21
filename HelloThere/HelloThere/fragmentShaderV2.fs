#version 330 core
in vec4 color;

out vec4 FragColor;

void main()
{
    // For constant shader, output a constant color (e.g., white)
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
