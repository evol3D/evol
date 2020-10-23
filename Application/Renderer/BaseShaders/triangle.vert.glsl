#version 450
#extension GL_EXT_nonuniform_qualifier : require






layout (push_constant) uniform pushConstants
{
  int index;
} push;








layout (binding = 0)  buffer Ver
{
  vec3 vecs[];
};








void main() 
{
  int a = push.index;
  gl_Position = vec4(vecs[nonuniformEXT(push.index)], 1);
}
