#version 330 core
out vec4 FragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;  

in vec3 Normal;
in vec3 FragPos;
in vec3 ViewPos;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    float specularStrength = 0.5;
    float shininess = 32;


    // normalizo la normal y la direccion de da luz
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos); // la direccion es dada por la diferencia entre la posicion del fragment y de la luz 

    vec3 viewDir = normalize(ViewPos - FragPos); // esto queda como fragpos -----> ViewPos = viewDir porque apunta hacia la misma
    vec3 reflectDir = reflect(-lightDir, norm);  // la fun reflect espera que la direccion de la luz apunte hacia la FragPos pero actualmente es al reves por el calculo previo

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;      

    // se calcula el angulo con dot product, cuanto mas amplio mas cercano a 0
    float diff = max(dot(norm, lightDir), 0.0); // podria ser negativo si el angulo > 90, por eso max()
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}