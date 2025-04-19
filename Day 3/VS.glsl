// VS.glsl
#version 330 core
layout(location = 1) in vec3 aPos;
layout(location = 2) in vec3 aColor;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;

out vec3 vertexColor;

void main() {
    gl_Position = projection_mat * view_mat * model_mat * vec4(aPos, 1.0);
    vertexColor = aColor;
}
