#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

//const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, 3.0, 1.0)); // normalize(vec3(1.0, -3.0, -1.0));
//const float AMBIENT = 0.03; // 0.1, 0.02;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);

    fragColor = inColor;
    //vec3 normalWordSpace = normalize(mat3(ubo.model) * inNormal);
    //float lightIntensity = AMBIENT + max(dot(normalWordSpace, DIRECTION_TO_LIGHT), 0);
    //fragColor = lightIntensity * inColor;

    fragTexCoord = inTexCoord;
}
