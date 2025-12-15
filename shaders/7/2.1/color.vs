#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out float spec;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;
uniform vec3 lightPos;


void main()
{
    // world space
    vec3 FragPos = vec3(model * vec4(aPos, 1.0));
    vec3 Normal  = normalize(mat3(transpose(inverse(model))) * aNormal);

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular (Gouraud)
    float specularStrength = 0.5;
    float shininess = 32.0;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vertexColor = (ambient + diffuse + specular) * objectColor;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}