#version 330 core

in vec3 ourColor;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D tex0;
uniform float useTexture;

void main()
{
    vec4 texColor = texture(tex0, texCoord);
    vec4 vertexColor = vec4(ourColor, 1.0);
    FragColor = mix(vertexColor, texColor, useTexture);
}
