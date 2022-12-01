#version 400
#define LIGHTS_NR 2

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
    sampler2D diffuse;
    sampler2D roughness;
    float shininess;
    sampler2D normalMap;  
    sampler2D emissionMap;
}; 
uniform Material material;

struct SpotLight {    
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
    float linear;
    float quadratic;  

    vec3 direction;
	float cutOff;
    bool enabled;
};  
uniform SpotLight spotLight;

vec3 PhongPointLightCalc(PointLight light, vec3 normal, vec3 ex_PositionEye, vec3 viewDirection)
{
        //Light position is in EYESPACE
        vec3 lightDir = normalize(light.position - ex_PositionEye);

        // diffuse shading
        float diff = max(dot(normal, lightDir), 0.0);

        // specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDirection, reflectDir), 0.0), 32);

        // attenuation
        float dist    = length(light.position - ex_PositionEye);
        float attenuation =  min(1.0 / (light.constant +
                                    light.linear * dist +
                                    light.quadratic * dist * dist)
                                    ,1);

        // combine results
        vec3 ambient  = light.ambient * vec3(texture(material.diffuse, ex_TexCoord));
        vec3 diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, ex_TexCoord));
        vec3 specular = light.specular * spec * vec3(texture(material.roughness, ex_TexCoord));
        vec3 emission = vec3(texture(material.emissionMap, ex_TexCoord));


        ambient  *= attenuation;
        diffuse  *= attenuation;
        specular *= attenuation;
        return (ambient + diffuse + specular + emission);
}

void main(void)
{

	//Calculate lighting
	vec3 color;

    //normal maps are broken so don't use
    vec3 normal = texture(material.normalMap, ex_TexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
   
    //fragment position is in eyespace
    for(int i = 0; i < LIGHTS_NR; i++)
    {
        color += PhongPointLightCalc(pointLights[i], normalize(ex_Normal), ex_PositionEye, normalize(glm::vec3(0) - ex_PositionEye));
    }

    //Add spotlight calculation
    if(spotLight.enabled)
    {
        vec3 lightDir = normalize(spotLight.position - ex_PositionEye);
        float theta = dot(lightDir, normalize(spotLight.direction));
        if(theta > spotLight.cutOff)
        {
            PointLight spotAsPoint;
            spotAsPoint.position = spotLight.position;

            spotAsPoint.ambient = spotLight.ambient;
            spotAsPoint.diffuse = spotLight.diffuse;
            spotAsPoint.specular = spotLight.specular;

            spotAsPoint.constant = spotLight.constant;
            spotAsPoint.linear =  spotLight.linear;
            spotAsPoint.quadratic = spotLight.quadratic;

            color += PhongPointLightCalc(spotAsPoint, normalize(ex_Normal), ex_PositionEye, normalize(glm::vec3(0) - ex_PositionEye));
        }
    }
    out_Color = vec4(color, 1.0);
}

