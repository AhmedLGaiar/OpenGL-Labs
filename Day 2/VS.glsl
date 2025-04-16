#version 330 core
layout(location = 1) in vec3 position;
layout(location = 2) in vec3 color;

uniform mat4 model_mat;
out vec3 vColor;

void main() {
    gl_Position = model_mat * vec4(position, 1.0);
    vColor = color;
}
