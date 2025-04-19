#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <gl/glm/glm.hpp>
#include <gl/glm/gtc/type_ptr.hpp>
#include <gl/glm/gtx/transform.hpp>

using namespace std;
using namespace glm;

GLuint InitShader(const char* vs, const char* fs);

struct Vertex {
    vec3 Position;
    vec3 Color;
};

const GLint WIDTH = 800, HEIGHT = 600;

GLuint BasicProgramId;
GLuint VBO_Cube, IBO_Cube;
GLuint model_mat_location, view_mat_location, projection_mat_location;

mat4 ModelMat, ViewMat, ProjectionMat;
float theta = 0;

// Camera
vec3 CameraPos = vec3(0.0f, 0.0f, 3.0f);
vec3 CameraFront = normalize(vec3(0.0f, 0.0f, -1.0f));
vec3 CameraUp = vec3(0.0f, 1.0f, 0.0f);
float camYaw = -90.0f, camPitch = 0.0f;

// Object rotation
float rotX = 0.0f, rotY = 0.0f;
float lastMouseX = 0.0f, lastMouseY = 0.0f;
bool rotating = false;

void CreateCube() {
    Vertex vertices[] = {
        {vec3(-0.5, 0.5, 0.5), vec3(1,0,0)}, {vec3(-0.5,-0.5, 0.5), vec3(0,1,0)},
        {vec3(0.5,-0.5, 0.5), vec3(1,0,1)}, {vec3(0.5, 0.5, 0.5), vec3(0,0,1)},
        {vec3(0.5, 0.5,-0.5), vec3(1,1,0)}, {vec3(0.5,-0.5,-0.5), vec3(0,1,1)},
        {vec3(-0.5,-0.5,-0.5), vec3(1,1,1)}, {vec3(-0.5, 0.5,-0.5), vec3(0,0,0)}
    };
    GLuint indices[] = {
        0,1,2, 0,2,3, 3,2,5, 3,5,4,
        4,5,6, 4,6,7, 7,6,1, 7,1,0,
        7,0,3, 7,3,4, 2,1,6, 2,6,5
    };

    glGenBuffers(1, &VBO_Cube);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &IBO_Cube);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_Cube);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
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
    vec3 smoothScale = mix(vec3(1.0f), scaleVec, 0.95f);

    ModelMat = translate(pos) *
        rotate(rotAngle, rotAxis) *
        scale(smoothScale);

    glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, value_ptr(ModelMat));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
}

int Init() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return 1;

    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    BasicProgramId = InitShader("VS.glsl", "FS.glsl");
    glUseProgram(BasicProgramId);

    model_mat_location = glGetUniformLocation(BasicProgramId, "model_mat");
    view_mat_location = glGetUniformLocation(BasicProgramId, "view_mat");
    projection_mat_location = glGetUniformLocation(BasicProgramId, "projection_mat");

    ProjectionMat = perspective(radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
    glUniformMatrix4fv(projection_mat_location, 1, GL_FALSE, value_ptr(ProjectionMat));

    CreateCube();
    return 0;
}

void Update(float dt) {
    theta += 1.0f * dt;

    float cameraSpeed = 2.5f * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        CameraPos += cameraSpeed * CameraFront;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        CameraPos -= cameraSpeed * CameraFront;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        CameraPos -= normalize(cross(CameraFront, CameraUp)) * cameraSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        CameraPos += normalize(cross(CameraFront, CameraUp)) * cameraSpeed;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
        CameraPos = vec3(0.0f, 0.0f, 3.0f);
        camYaw = -90.0f;
        camPitch = 0.0f;
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(BasicProgramId);

    ViewMat = lookAt(CameraPos, CameraPos + CameraFront, CameraUp);
    glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, value_ptr(ViewMat));
    BindCube();

    float angle = sin(theta);
    DrawPart(vec3(0, 0, 0), vec3(0, 1, 0), 0, vec3(1, 0.5, 0.5));                // Body
    DrawPart(vec3(0.6f, 0.3f, 0), vec3(0, 1, 0), angle * 45.0f, vec3(0.3f));      // Head
    DrawPart(vec3(-0.7f, 0.2f, 0), vec3(0, 1, 0), angle * 60.0f, vec3(0.1f, 0.1f, 0.4f)); // Tail
    DrawPart(vec3(0.4f, -0.4f, 0.2f), vec3(1, 0, 0), angle * 15, vec3(0.1f, 0.4f, 0.1f)); // Legs
    DrawPart(vec3(0.4f, -0.4f, -0.2f), vec3(1, 0, 0), -angle * 15, vec3(0.1f, 0.4f, 0.1f));
    DrawPart(vec3(-0.4f, -0.4f, 0.2f), vec3(1, 0, 0), -angle * 15, vec3(0.1f, 0.4f, 0.1f));
    DrawPart(vec3(-0.4f, -0.4f, -0.2f), vec3(1, 0, 0), angle * 15, vec3(0.1f, 0.4f, 0.1f));
}


template<typename T>
T myClamp(T value, T minVal, T maxVal) {
    return (value < minVal) ? minVal : (value > maxVal) ? maxVal : value;
}


int main() {
    sf::ContextSettings context;
    context.depthBits = 24;
    sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "OpenGL Dog - First Person View", sf::Style::Close, context);
    window.setActive(true);
    window.setMouseCursorGrabbed(true);
    window.setMouseCursorVisible(false);
    sf::Mouse::setPosition(sf::Vector2i(WIDTH / 2, HEIGHT / 2), window);

    if (Init()) return -1;
    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();

            if (e.type == sf::Event::MouseWheelScrolled)
                CameraPos += e.mouseWheelScroll.delta * 1.0f * CameraFront;
        }

        // Mouse look - first person
        sf::Vector2i center(WIDTH / 2, HEIGHT / 2);
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        float deltaX = mousePos.x - center.x;
        float deltaY = center.y - mousePos.y;

        float sensitivity = 0.05f;
        camYaw += deltaX * sensitivity;
        camPitch += deltaY * sensitivity;
        camPitch = myClamp(camPitch, -89.0f, 89.0f);

        vec3 direction;
        direction.x = cos(radians(camYaw)) * cos(radians(camPitch));
        direction.y = sin(radians(camPitch));
        direction.z = sin(radians(camYaw)) * cos(radians(camPitch));
        CameraFront = normalize(direction);

        sf::Mouse::setPosition(center, window);

        Update(dt);
        Render();
        window.display();
    }

    return 0;
}

