#version 330

layout (location = 1) in vec4 vertex_position;
layout (location = 2) in vec4 vertex_normal;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;

// fixed but can be uniform
// vectors
vec3 camera_position = vec3(0,0,3);
vec4 object_color = vec4(1,1,1,1);
vec4 light_color = vec4(0.6,0.1,1,1);
vec3 light_direction = vec3(1,1,1); // to make calculations easier

// fixed but can be uniform
// lights coefficient
float ka = 0.5;
float kd = 0.8;
float ks = 5;
float sh = 30;

// main lights vectors
vec3 v;
vec3 n;
vec3 l;
vec3 h;

out vec4 frag_color;
void main()
{
	gl_Position = projection_mat * view_mat * model_mat * vertex_position;

	// lights calculations

	l = normalize(light_direction);
	n = normalize((normalize(model_mat * vertex_normal)).xyz);
	v = normalize(camera_position - (model_mat * vertex_position).xyz);
	h = normalize(v+l);

	//float I = ka +
	//kd * max(dot(l,n),0) +
	//ks * max(pow(dot(h,n),sh),0);
	//vec4 light_final_color = I * light_color;

	vec4 light_final_color = ka * light_color +
	kd * max(dot(l,n),0) * light_color +
	ks * max(pow(dot(h,n),sh),0) * light_color;

	frag_color = object_color * light_final_color;
}