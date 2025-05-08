#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 3) in vec3 instancePos;
layout (location = 4) in float instanceScale;

uniform mat4 camMatrix;

void main() {
    mat4 scaleMat = mat4(
        vec4(instanceScale, 0.0, 0.0, 0.0),
        vec4(0.0, instanceScale, 0.0, 0.0),
        vec4(0.0, 0.0, instanceScale, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );

    mat4 translateMat = mat4(1.0);
    translateMat[3] = vec4(instancePos, 1.0);

    mat4 localModel = translateMat * scaleMat;

    gl_Position = camMatrix * localModel * vec4(aPos, 1.0);
}
