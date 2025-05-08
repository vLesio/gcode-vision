#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;

uniform mat4 camMatrix;
uniform mat4 model;

out vec3 ourColor;
out vec2 texCoord;

void main() {
    mat4 localModel = model;
    gl_Position = camMatrix * localModel * vec4(aPos, 1.0);
    ourColor = aColor;
    texCoord = aTex;
}
