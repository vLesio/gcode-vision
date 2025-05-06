#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;

layout (location = 3) in vec3 instancePos;
layout (location = 4) in float instanceScale;

uniform mat4 camMatrix;
uniform mat4 model;
uniform bool isInstanced;

out vec3 ourColor;
out vec2 texCoord;

void main() {
    mat4 localModel = mat4(1.0);

    if (isInstanced) {
        localModel = mat4(1.0);
        localModel[3] = vec4(instancePos, 1.0); // translate
        localModel = localModel * mat4(         // scale
            vec4(instanceScale, 0.0, 0.0, 0.0),
            vec4(0.0, instanceScale, 0.0, 0.0),
            vec4(0.0, 0.0, instanceScale, 0.0),
            vec4(0.0, 0.0, 0.0, 1.0)
        );
    } else {
        localModel = model;
    }

    gl_Position = camMatrix * localModel * vec4(aPos, 1.0);
    ourColor = aColor;
    texCoord = aTex;
}
