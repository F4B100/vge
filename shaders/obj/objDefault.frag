#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec3 fragPos;
layout(location = 2) in vec2 fragTexCoords;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D tex;
layout(binding = 2) uniform sampler2D tex1;
layout(binding = 3) uniform Shininess {
    float shininess;
} shininess;

layout(binding = 4) uniform LightInfo {
    vec3 pos;
    vec3 color;
}light;

layout(binding = 5) uniform CameraInfo {
    vec3 pos;
}camera;

void main() {
    float ambientStrength = 0.9;
    vec3 ambient = ambientStrength * light.color;

    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(light.pos - fragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color;

    vec3 result = (ambient * diffuse) * texture(tex, fragTexCoords).rgb;
    outColor = vec4(texture(tex, fragTexCoords).rgb, 1.0f);
}
