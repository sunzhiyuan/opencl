#version 310 es

uniform sampler2D Diffuse;

in vec2 VertTexcoord;

out vec4 Color;

void main()
{
	Color = texture(Diffuse, VertTexcoord);
}
