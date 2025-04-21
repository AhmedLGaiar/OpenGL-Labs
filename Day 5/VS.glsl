#version 330

layout(location = 1) in vec4 vertex_position;
layout(location = 2) in vec4 vertex_color;

//uniform float theta;
uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;

out vec4 frag_color;
void main()
{
	//gl_Position = vec4(vertex_position.x/2, vertex_position.y/2, 0, 1);

	// around Z
	//float x = vertex_position.x * cos(theta) + vertex_position.y * -sin(theta);
	//float y = vertex_position.x * sin(theta) + vertex_position.y * cos(theta);

	// around Y
	//float x = vertex_position.x * cos(theta) + vertex_position.z * sin(theta);
	//float y = vertex_position.y;
	//float z = -vertex_position.x * sin(theta) + vertex_position.z * cos(theta);

	//gl_Position = vec4(x/2,y/2,z/2,1);

	gl_Position = projection_mat * view_mat * model_mat * vertex_position;

	frag_color = vertex_color;
}