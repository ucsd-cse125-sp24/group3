#version 330 core
in vec2 TexCoords
out vec4 color

// Reference: The chapter on sprite rendering from
// https://learnopengl.com/book/book_pdf.pdf

uniform sampler2D image;
uniform vec3 spriteColor;

void main()
{
    color = texture(image, TexCoords);
}
