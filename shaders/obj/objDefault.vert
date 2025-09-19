#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec3 fragPos;
layout(location = 2) out vec2 fragTexCoords;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    vec4 pos = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    pos.y = -pos.y;
    gl_Position = pos;
    fragNormal = inNormal;
    fragPos = pos.xyz;
    fragTexCoords = inTexCoords;
}
