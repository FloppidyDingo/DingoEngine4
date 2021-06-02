#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in float opacity;

out vec2 TexCoord;
out vec2 FragPos;
out float opac;

void main()
{
    gl_Position = vec4(pos, 1.0); // see how we directly give a vec3 to vec4's constructor
	FragPos = vec3(vec4(pos, 1.0)).xy;
	TexCoord = aTexCoord;
	opac = opacity;
}