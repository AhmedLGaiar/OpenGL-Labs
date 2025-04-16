#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <gl/glm/glm.hpp>
#include <gl/glm/gtc/type_ptr.hpp>
#include <gl/glm/gtx/transform.hpp>

using namespace std;
using namespace glm;

struct Vertex {
    vec3 Position;
    vec3 Color;
};

const GLint WIDTH = 800, HEIGHT = 600;
GLuint BasicProgramId;
GLuint VBO_Cube, IBO_Cube;
GLuint model_mat_location;

mat4 ModelMat;
float theta = 0;

GLuint InitShader(const char* vs_file, const char* fs_file); // Declare external

void CreateCube() {
    Vertex vertices[] = {
        {vec3(-0.5, 0.5, 0.5), vec3(1,0,0)},
        {vec3(-0.5,-0.5, 0.5), vec3(0,1,0)},
        {vec3(0.5,-0.5, 0.5), vec3(1,0,1)},
        {vec3(0.5, 0.5, 0.5), vec3(0,0,1)},
        {vec3(0.5, 0.5,-0.5), vec3(1,1,0)},
        {vec3(0.5,-0.5,-0.5), vec3(0,1,1)},
        {vec3(-0.5,-0.5,-0.5), vec3(1,1,1)},
        {vec3(-0.5, 0.5,-0.5), vec3(0,0,0)}
    };

    GLuint indices[] = {
        0,1,2, 0,2,3,
        3,2,5, 3,5,4,
        4,5,6, 4,6,7,
        7,6,1, 7,1,0,
        7,0,3, 7,3,4,
        2,1,6, 2,6,5
    };

    glGenBuffers(1, &VBO_Cube);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO_Cube);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_Cube);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(vec3));
    glEnableVertexAttribArray(2);
}

void BindCube() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_Cube);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(vec3));
    glEnableVertexAttribArray(2);
}

void DrawPart(vec3 pos, vec3 rotAxis, float rotAngle, vec3 scaleVec) {
    ModelMat = translate(pos) * rotate(rotAngle, rotAxis) * scale(scaleVec);
    glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, value_ptr(ModelMat));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
}

int Init() {
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        cout << "Error initializing GLEW\n";
        return 1;
    }

    cout << "OpenGL Info:\n";
    cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
    cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
    cout << "Version: " << glGetString(GL_VERSION) << "\n";
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

    BasicProgramId = InitShader("VS.glsl", "FS.glsl");
    glUseProgram(BasicProgramId);

    CreateCube();
    model_mat_location = glGetUniformLocation(BasicProgramId, "model_mat");

    glClearColor(0.1f, 0.1f, 0.1f, 1);
    glEnable(GL_DEPTH_TEST);
    return 0;
}

void Update() {
    theta += 0.01f;
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    BindCube();

    float angle = sin(theta);

    // Body
    DrawPart(vec3(0, 0, 0), vec3(0, 1, 0), 0, vec3(1, 0.5, 0.5));

    // Head
    DrawPart(vec3(0.6f, 0.3f, 0), vec3(0, 1, 0), angle * 45.0f, vec3(0.3f, 0.3f, 0.3f));

    // Tail
    DrawPart(vec3(-0.7f, 0.2f, 0), vec3(0, 1, 0), angle * 60.0f, vec3(0.1f, 0.1f, 0.4f));

    // Legs
    DrawPart(vec3(0.4f, -0.4f, 0.2f), vec3(1, 0, 0), angle * 15, vec3(0.1f, 0.4f, 0.1f));
    DrawPart(vec3(0.4f, -0.4f, -0.2f), vec3(1, 0, 0), -angle * 15, vec3(0.1f, 0.4f, 0.1f));
    DrawPart(vec3(-0.4f, -0.4f, 0.2f), vec3(1, 0, 0), -angle * 15, vec3(0.1f, 0.4f, 0.1f));
    DrawPart(vec3(-0.4f, -0.4f, -0.2f), vec3(1, 0, 0), angle * 15, vec3(0.1f, 0.4f, 0.1f));
}

int main() {
    sf::ContextSettings context;
    context.depthBits = 24;
    sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "OpenGL Dog", sf::Style::Close, context);

    if (Init()) return -1;

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();
        }

        Update();
        Render();
        window.display();
    }

    return 0;
}
