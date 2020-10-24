#version 450

/* #extension GL_EXT_nonuniform_qualifier : require */






/* layout (push_constant) uniform PushConstant */
/* { */
/*   int vertexBufferIndex; */
/* } RenderData; */








layout (binding = 0) buffer VertexBuffers
{
  layout (align = 16) vec3 vertices[];
};








void main() 
{
  gl_Position = vec4(vertices[gl_VertexIndex], 1);
}
