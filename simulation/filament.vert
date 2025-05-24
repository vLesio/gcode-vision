#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

layout (location = 3) in vec3 instancePos;
layout (location = 4) in vec3 instanceScale;
layout (location = 5) in vec4 instanceRotation;

uniform mat4 camMatrix;

// Converts a quaternion to a 4x4 rotation matrix
mat4 quatToMat4(vec4 q) {
    float x = q.x, y = q.y, z = q.z, w = q.w;
    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;
    float xx = x * x2;
    float xy = x * y2;
    float xz = x * z2;
    float yy = y * y2;
    float yz = y * z2;
    float zz = z * z2;
    float wx = w * x2;
    float wy = w * y2;
    float wz = w * z2;

    return mat4(
        vec4(1.0 - (yy + zz), xy + wz, xz - wy, 0.0),
        vec4(xy - wz, 1.0 - (xx + zz), yz + wx, 0.0),
        vec4(xz + wy, yz - wx, 1.0 - (xx + yy), 0.0),
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
    vec4 worldPosition = model * vec4(aPos, 1.0);
    FragPos = vec3(worldPosition);
    
    Normal = mat3(transpose(inverse(model))) * aNormal; 

    gl_Position = camMatrix * worldPosition;
}
