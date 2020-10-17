#version 450

layout (binding = 0) buffer Vertices
{
  layout (align = 16) vec3 vertices[];
};

void main() 
{
  gl_Position = vec4(vertices[gl_VertexIndex], 1);
}
