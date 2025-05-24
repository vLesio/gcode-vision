#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor = vec3(1.0);
uniform vec3 objectColor;
uniform vec3 viewPos;

uniform float useTexture;
uniform float shininess;
uniform float ambientStrength;
uniform float specularStrength;

uniform sampler2D tex0;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    // Ambient
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 texColor = texture(tex0, TexCoord).rgb;
    vec3 baseColor = mix(objectColor, texColor, clamp(useTexture, 0.0, 1.0));
    vec3 result = (ambient + diffuse + specular) * baseColor;

    FragColor = vec4(result, 1.0);
}
