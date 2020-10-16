#version 450

/* const vec3 vertices[] = */ 
/* { */
/*     vec3(0, 0.5, 0), */
/*     vec3(-0.5, -0.5, 0), */
/*     vec3(0.5, -0.5, 0), */
/* }; */
layout (binding = 0) buffer Vertices
{
  vec4 vertices[];
};

void main() 
{
  gl_Position = vertices[gl_VertexIndex];
}
