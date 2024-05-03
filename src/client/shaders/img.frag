#version 330 core
in vec2 TexCoords
out vec4 color

// Reference: The chapter on sprite rendering from
// https://learnopengl.com/book/book_pdf.pdf

uniform sampler2D image;
uniform vec3 spriteColor;

void main()
{
    color = vec4(spriteColor, 1.0) * texture(image, TexCoords);
}