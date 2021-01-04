#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout (push_constant) uniform PushConstant
{
  int vertexBufferIndex;
  int normalBufferIndex;
  int uvBufferIndex;

  int materialIndex;

  mat4 model;
} RenderData;

layout(set = 2, binding = 0) uniform sampler2D texSampler[];

layout(location = 0) in vec2 uv;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec4 outputColor;

vec3 lightDir = vec3(1, 0, 1);
float ambientLight = 0.1;

void main()
{
    vec4 color = vec4(texture(texSampler[RenderData.materialIndex],uv).xyz,1);

    vec3 normalizedLight = normalize(lightDir);
    vec3 rotatedNormal = vec3(RenderData.model * vec4(normal, 0));
    float lightIntensity = dot(normalizedLight, normalize(rotatedNormal)) + ambientLight;

    color *= lightIntensity;

    outputColor = color;
}
