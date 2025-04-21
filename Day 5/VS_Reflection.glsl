#version 330

layout(location = 1) in vec4 vertex_position;
layout(location = 2) in vec4 vertex_normal;
layout(location = 3) in vec2 vertex_uv;

//uniform float theta;
uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;

out vec3 frag_position;
out vec3 frag_normal;
void main()
{
	gl_Position = projection_mat * view_mat * model_mat * vertex_position;

	frag_position = vertex_position.xyz;
	frag_normal = vertex_normal.xyz;
}