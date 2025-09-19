#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec3 fragPos;
layout(location = 2) in vec2 fragTexCoords;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D tex;

void main() {
    outColor = vec4(fragNormal,1.0f) * texture(tex, fragTexCoords);
}
