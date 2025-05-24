#version 330 core

in vec3 ourColor;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D tex0;
uniform float useTexture;
uniform vec3 viewPos;
uniform vec3 lightPos = vec3(1.0, 5.0, 1.0);  
uniform vec3 lightColor = vec3(1.0);         

void main()
{
    vec4 texColor = texture(tex0, texCoord);
    vec4 vertexColor = vec4(ourColor, 1.0);
    FragColor = mix(vertexColor, texColor, useTexture);
}
