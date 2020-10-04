#version 450

struct Vertex
{
    float vx, vy, vz;
    float nx, ny, nz;
    float tu, tv;
};

layout(binding = 0) buffer Vertices
{
	Vertex vertices[];
};

layout(location = 0) out vec4 color;

void main() 
{
	Vertex v = vertices[gl_VertexIndex];

    vec3 position = vec3(v.vx, v.vy, v.vz);
    vec3 normal   = vec3(v.nx, v.ny, v.nz);
    vec2 uv       = vec2(v.tu, v.tv);

    float scale = 1.0f;
    gl_Position =  vec4(position.x * scale, position.y * scale, position.z * 0.5f + 0.5f, 1.0f);
    //gl_Position =  vec4(position, 1.0f);
	color = vec4(normal, 1.0f);
}
