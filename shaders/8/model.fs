#version 330 core
out vec4 FragColor;

struct SpotLight {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;

    float cutOffAngle;
    float outerCutOff;
};

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    // sampler2D emission1;
    // float shininess;    
};

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

uniform SpotLight spotLight;
uniform Material material;

in vec2 TexCoord;
in vec3 normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{    
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);
    

    result += calcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
     // ambient
    vec3 ambient = light.ambient * texture(material.texture_specular1, TexCoord).rgb;

    // diffuse 
   
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoord).rgb;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * spec * texture(material.texture_specular1, TexCoord).rgb;  


    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOffAngle - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return diffuse + specular + ambient;
}