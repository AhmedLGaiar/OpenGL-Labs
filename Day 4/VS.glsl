#version 330 core
layout(location = 1) in vec3 aPos;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec3 aNormal;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;

void main() {
    FragPos = vec3(model_mat * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model_mat))) * aNormal;
    Color = aColor;
    gl_Position = projection_mat * view_mat * vec4(FragPos, 1.0);
}
