#version 330

layout(location = 1) in vec4 vertex_position;
layout(location = 2) in vec4 vertex_normal;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;

// fixed but can be uniform
// vectors
vec3 camera_position = vec3(0,0,3);
vec3 light_direction = vec3(1,1,1); // to make calculations easier

// main lights vectors
out vec3 v;
out vec3 n;
out vec3 l;
out vec3 h;

void main()
{
	gl_Position = projection_mat * view_mat * model_mat * vertex_position;

	l = normalize(light_direction);
	n = normalize((normalize(model_mat * vertex_normal)).xyz);
	v = normalize(camera_position - (model_mat * vertex_position).xyz);
	h = normalize(v+l);
}