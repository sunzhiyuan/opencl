#version 310 es

#define POSITION		0
#define COLOR			3
#define FRAG_COLOR		0
#define TRANSFORM0	1

uniform transform
{
	mat4 MVP;
	mat4 DepthMVP;
	mat4 DepthBiasMVP;
} Transform;

layout(location = POSITION) in vec3 Position;

void main()
{	
	gl_Position = Transform.DepthMVP * vec4(Position, 1.0);
}
