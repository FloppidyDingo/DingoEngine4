#version 330
#define MAX_LIGHTS 24

uniform sampler2D tile;

in vec2 TexCoord;

out vec4 FragColor;

//prototype for point light function

void main(){
	FragColor = FragColor * texture(tile, TexCoord);
}