#version 330 core
out vec4 FragColor;
   
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;
uniform float time;


in vec3 Normal;
in vec3 FragPos;
in vec2 TextCoord;

void main()
{
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TextCoord).rgb;

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TextCoord).rgb;

    float distance = length(light.position - FragPos);
    float attenuation = 1/(light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TextCoord).rgb;  

    vec3 emission = vec3(0.0);

     if (texture(material.specular, TextCoord).r == 0.0)   /*rough check for blackbox inside spec texture */
    {
        /*apply emission texture */
        emission = texture(material.emission, TextCoord).rgb;
        
        // /*some extra fun stuff with "time uniform" */
        emission = texture(material.emission, TextCoord + vec2(0.0,time)).rgb;   /*moving */
        // emission = emission * (sin(time) * 0.5 + 0.5) * 2.0;                     /*fading */
    }


    ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation;   

    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0);
}