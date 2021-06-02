#version 330
#define MAX_LIGHTS 24

uniform sampler2D tile;
uniform float[(MAX_LIGHTS * 8) + 1] Lights; //Light format: Type R G B b x y r

in vec2 TexCoord;
in vec2 FragPos;

out vec4 FragColor;

vec4 temp4;
vec4 lightColor;
vec2 temp2;
float tempf;
float temptype;

//prototype for point light function
vec4 point(vec4 color, vec2 pos, float radius);

void main(){
	for(int i = 0; i < MAX_LIGHTS * 8; i++){
		temptype = Lights[i];
		switch (int(temptype)){
			case 0:{ //NOP
				i += 7;
				break;
			}
			case 1:{ //ambient
				temp4.r = Lights[i + 1];
				temp4.g = Lights[i + 2];
				temp4.b = Lights[i + 3];
				temp4.a = Lights[i + 4];
				lightColor = mix(lightColor, temp4, temp4.a);
				i += 7;
				break;
			}
			case 2:{ //point light
				temp4.r = Lights[i + 1];
				temp4.g = Lights[i + 2];
				temp4.b = Lights[i + 3];
				temp4.a = Lights[i + 4];
				temp2.x = Lights[i + 5];
				temp2.y = Lights[i + 6];
				tempf = Lights[i + 7];
				if(sqrt(pow(FragPos.x - temp2.x, 2) + pow(FragPos.y - temp2.y, 2)) < tempf){
					lightColor = mix(lightColor, temp4, temp4.a);
				}
				i += 7;
				break;
			}
			case 3:{ //light exit
				i = MAX_LIGHTS * 8;
				break;
			}
		}
	}
	FragColor = lightColor * texture(tile, TexCoord);
}