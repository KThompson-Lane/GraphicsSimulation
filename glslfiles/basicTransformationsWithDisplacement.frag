#version 400
#define LIGHTS_NR 1
in  vec2 ex_TexCoord; //texture coord arriving from the vertex
in  vec3 ex_Normal;  //normal arriving from the vertex

out vec4 out_Color;   //colour for the pixel

in vec3 ex_PositionEye; //Fragment position in eye space arriving from the vertex shader

struct PointLight {    
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
    float linear;
    float quadratic;  
};  
uniform PointLight pointLights[LIGHTS_NR];

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

uniform sampler2D DiffuseMap;


vec3 PhongPointLightCalc(PointLight light, vec3 normal, vec3 ex_PositionEye, vec3 viewDirection)
{
        //Light position is in EYESPACE
        vec3 lightDir = normalize(light.position - ex_PositionEye);

        // diffuse shading
        float diff = max(dot(normal, lightDir), 0.0);

        // specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDirection, reflectDir), 0.0), material.shininess);

        // attenuation
        float dist    = length(light.position - ex_PositionEye);
        float attenuation =  min(1.0 / (light.constant +
                                    light.linear * dist +
                                    light.quadratic * dist * dist)
                                    ,1);

        // combine results
        vec3 ambient  = light.ambient * material.ambient  * vec3(texture(DiffuseMap, ex_TexCoord));
        vec3 diffuse  = light.diffuse * material.diffuse * diff * vec3(texture(DiffuseMap, ex_TexCoord));
        vec3 specular = light.specular * material.specular * spec * vec3(texture(DiffuseMap, ex_TexCoord));
        ambient  *= attenuation;
        diffuse  *= attenuation;
        specular *= attenuation;
        return (ambient + diffuse + specular);
}

void main(void)
{

	//Calculate lighting
	vec3 color;

    //fragment position is in eyespace
    for(int i = 0; i < LIGHTS_NR; i++)
    {
        color += PhongPointLightCalc(pointLights[i], normalize(ex_Normal), ex_PositionEye, normalize(ex_PositionEye));
    }

   out_Color = vec4(color, 1.0) * texture(DiffuseMap, ex_TexCoord); //show texture and lighting
}

