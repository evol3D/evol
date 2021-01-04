#version 450

layout(location = 0) in vec4 color;
layout(location = 0) out vec4 outputColor;

layout(set = 2, binding = 0) uniform sampler2D texSampler[];

void main()
{

    outputColor = color;
}
