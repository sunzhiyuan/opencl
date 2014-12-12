#version 310 es

uniform sampler2D Diffuse;
uniform sampler2DShadow Shadow;

#define POSITION		0
#define COLOR			3
#define FRAG_COLOR		0
#define TRANSFORM0	1

layout(location = 5) uniform int uniformExtensionTest = 0;

uniform float interpolator[2][4] = {float[4](0.5, 0.5, 0.5, 0.5), float[4](1.0, 1.0, 1.0, 1.0)};

in block
{
	vec4 Color;
	vec4 ShadowCoord;
} In;

layout(location = FRAG_COLOR) out vec4 Color;

void main()
{
	vec4 Diffuse = In.Color;

	ivec2 ShadowSize = textureSize(Shadow, 0);
	vec4 ShadowCoord = In.ShadowCoord;
	ShadowCoord.z -= 0.005;

	vec4 Gather = textureGather(Shadow, ShadowCoord.xy, ShadowCoord.z);

	vec2 TexelCoord = ShadowCoord.xy * vec2(ShadowSize.xy);
	vec2 SampleCoord = fract(TexelCoord + 0.5);

	float Texel0 = mix(Gather.x, Gather.y, SampleCoord.x);
	float Texel1 = mix(Gather.w, Gather.z, SampleCoord.x);
	float Visibility = mix(Texel1, Texel0, SampleCoord.y);

	if (uniformExtensionTest == 1)
	{
		Color = vec4(mix(vec4(interpolator[0][0], interpolator[0][1], interpolator[0][2], interpolator[0][3]), vec4(interpolator[1][0], interpolator[1][1], interpolator[1][2], interpolator[1][3]), Visibility) * Diffuse);
	}
	else
	{
		Color = vec4(0.0);
	}
}
