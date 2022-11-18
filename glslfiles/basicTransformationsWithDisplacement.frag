#version 400

in  vec2 ex_TexCoord; //texture coord arriving from the vertex
in  vec3 ex_Normal;  //normal arriving from the vertex

out vec4 out_Color;   //colour for the pixel
in vec3 ex_LightDir;  //light direction arriving from the vertex

in vec3 ex_PositionEye; //Fragment position in eye space arriving from the vertex shader

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;  

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

uniform sampler2D DiffuseMap;

void main(void)
{
	//Calculate lighting
	vec3 n, L;
	vec3 color;
	float NdotL;
	
	n = normalize(ex_Normal);
	L = normalize(ex_LightDir);

	vec3 v = normalize(-ex_PositionEye);
	vec3 r = normalize(-reflect(L, n));
	
	float RdotV = max(0.0, dot(r, v));

	NdotL = max(dot(n, L),0.0);
	color = light.ambient * material.ambient;
	
	if(NdotL > 0.0) 
	{
		color += (light.ambient * material.diffuse * NdotL);
	}

	color += material.specular * light.specular * pow(RdotV, material.shininess);

    out_Color = vec4(color, 1.0) * texture(DiffuseMap, ex_TexCoord); //show texture and lighting
}