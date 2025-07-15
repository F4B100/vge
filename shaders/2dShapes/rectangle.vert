#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoords;

layout(binding = 0) uniform UniformBufferObject {
    vec2 ScreenPos;
} ubo;

void main() {
    vec2 pos = inPosition + ubo.ScreenPos;
    gl_Position = vec4(pos, 0.0f, 1.0f);

    fragTexCoords = inTexCoords;
    fragColor = vec3(inTexCoords, 0.0f);
}
