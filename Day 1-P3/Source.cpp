#include <iostream>
#include <vector>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

const GLint WIDTH = 800, HEIGHT = 600;
GLuint shaderProgram, VAO, VBO;

const char* vertexShaderCode = R"(
#version 330 core
layout(location = 0) in float xPos;

void main()
{
    float y = sin(xPos * 20.0) * 0.4 + sin(xPos * 60.0) * 0.2;
    gl_Position = vec4(xPos, y, 0.0, 1.0);
}
)";

const char* fragmentShaderCode = R"(
#version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(0.1, 0.1, 1.0, 1.0);
}
)";

GLuint CompileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    return shader;
}

void InitGL()
{
    glewInit();

    GLuint vShader = CompileShader(GL_VERTEX_SHADER, vertexShaderCode);
    GLuint fShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderCode);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, fShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    // Generate X positions
    std::vector<float> xCoords;
    for (float x = -1.0f; x <= 1.0f; x += 0.005f)
        xCoords.push_back(x);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, xCoords.size() * sizeof(float), xCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
}

int main()
{
    sf::ContextSettings settings;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.depthBits = 24;

    sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "Vertex Shader Sin Wave", sf::Style::Close, settings);
    window.setVerticalSyncEnabled(true);

    InitGL();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_STRIP, 0, 400);

        window.display();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}