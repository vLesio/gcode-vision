#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightPos = vec3(1.0, 5.0, 1.0);    // Light position in world space
uniform vec3 viewPos;                           // Camera position in world space
uniform vec3 lightColor = vec3(1.0);            // Light color (white by default)
//uniform vec3 objectColor = vec3(1.0, 0.88, 0.75); // Filament color - cream
//uniform vec3 objectColor = vec3(0.3, 0.9, 1.0); // light blue
uniform vec3 objectColor = vec3(1.0f, 0.6f, 0.3f); // light orange

void main()
{

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.4;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}