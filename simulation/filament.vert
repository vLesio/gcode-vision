#version 330 core

layout (location = 0) in vec3 aPos;

layout (location = 3) in vec3 instancePos;
layout (location = 4) in vec3 instanceScale;
layout (location = 5) in vec4 instanceRotation;

uniform mat4 camMatrix;

// Converts a quaternion to a 4x4 rotation matrix
mat4 quatToMat4(vec4 q) {
    float x = q.x, y = q.y, z = q.z, w = q.w;
    return mat4(
        vec4(1 - 2*y*y - 2*z*z,     2*x*y - 2*w*z,     2*x*z + 2*w*y, 0.0),
        vec4(2*x*y + 2*w*z, 1 - 2*x*x - 2*z*z,     2*y*z - 2*w*x, 0.0),
        vec4(2*x*z - 2*w*y,     2*y*z + 2*w*x, 1 - 2*x*x - 2*y*y, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );
}

void main() {
    mat4 scaleMat = mat4(1.0);
    scaleMat[0][0] = instanceScale.x;
    scaleMat[1][1] = instanceScale.y;
    scaleMat[2][2] = instanceScale.z;

    mat4 rotMat = quatToMat4(instanceRotation);

    mat4 translateMat = mat4(1.0);
    translateMat[3] = vec4(instancePos, 1.0);

    mat4 model = translateMat * rotMat * scaleMat;

    gl_Position = camMatrix * model * vec4(aPos, 1.0);
}
