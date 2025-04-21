#include <iostream>
#include <vector>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <gl/glm/glm.hpp>
#include <gl/glm/gtc/type_ptr.hpp>
#include <gl/glm/gtx/transform.hpp>

using namespace std;
using namespace glm;

enum DrawingMode { Points, Lines, FilledTriangle };

struct Vertex { vec3 Position; vec3 Color; vec3 Normal; };

const GLint WIDTH = 800, HEIGHT = 600;

DrawingMode Current_DrawingMode = DrawingMode::FilledTriangle;

GLuint BasicProgramId;
GLuint VBO_Triangle, VBO_ColoredTriangle, VBO_Cube, VBO_Sky, VBO_Sphere;
GLuint IBO_Cube, IBO_Sphere;
GLuint Theta_Location, model_mat_location, view_mat_location, projection_mat_location;
GLuint lightPos_location, lightColor_location, viewPos_location;
mat4 ModelMat, ViewMat, ProjectionMat;

vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

float cameraYaw = -90.0f;
float cameraPitch = 0.0f;

float lastX = WIDTH / 2.0f, lastY = HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool middleMouseHeld = false;
bool resetViewRequested = false;
float baseSpeed = 2.5f;

vec3 lightPos = vec3(1.2f, 1.0f, 2.0f);
vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name);


void CreateColoredTriangle() {
    Vertex TriangleVertices[] = {
        {vec3(-1, -1, 0), vec3(1.0f, 0.2f, 0.2f), vec3(0, 0, 1)},
        {vec3(1, -1, 0), vec3(0.2f, 1.0f, 0.2f), vec3(0, 0, 1)},
        {vec3(0, 1, 0), vec3(0.2f, 0.2f, 1.0f), vec3(0, 0, 1)}
    };

    glGenBuffers(1, &VBO_ColoredTriangle);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_ColoredTriangle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleVertices), TriangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(sizeof(vec3)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(2 * sizeof(vec3)));
    glEnableVertexAttribArray(3);
}

void BindColoredTriangle() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO_ColoredTriangle);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(sizeof(vec3)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(2 * sizeof(vec3)));
    glEnableVertexAttribArray(3);
}

void CreateCube() {
    Vertex cube_core_vertices[] = {
        {vec3(-0.5, 0.5, 0.5), vec3(1.0f, 0.6f, 0.6f), vec3(0, 0, 1)},
        {vec3(-0.5, -0.5, 0.5), vec3(1.0f, 0.6f, 0.6f), vec3(0, 0, 1)},
        {vec3(0.5, -0.5, 0.5), vec3(1.0f, 0.6f, 0.6f), vec3(0, 0, 1)},
        {vec3(0.5, 0.5, 0.5), vec3(1.0f, 0.6f, 0.6f), vec3(0, 0, 1)},
        {vec3(0.5, 0.5, -0.5), vec3(0.4f, 0.4f, 1.0f), vec3(0, 0, -1)},
        {vec3(0.5, -0.5, -0.5), vec3(0.4f, 0.4f, 1.0f), vec3(0, 0, -1)},
        {vec3(-0.5, -0.5, -0.5), vec3(0.4f, 0.4f, 1.0f), vec3(0, 0, -1)},
        {vec3(-0.5, 0.5, -0.5), vec3(0.4f, 0.4f, 1.0f), vec3(0, 0, -1)}
    };
    int vertices_Indeces[] = {
        0,1,2, 0,2,3, 3,2,5, 3,5,4, 4,5,6, 4,6,7,
        7,6,1, 7,1,0, 7,0,3, 7,3,4, 2,1,6, 2,6,5
    };
    glGenBuffers(1, &VBO_Cube);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_core_vertices), cube_core_vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &IBO_Cube);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_Cube);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices_Indeces), vertices_Indeces, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(sizeof(vec3)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(2 * sizeof(vec3)));
    glEnableVertexAttribArray(3);
}

void BindCube() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_Cube);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(sizeof(vec3)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(2 * sizeof(vec3)));
    glEnableVertexAttribArray(3);
}

void CreateSphere(float radius, int sectors, int stacks) {
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    float sectorStep = 2 * 3.14159265359f / sectors;
    float stackStep = 3.14159265359f / stacks;

    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = 3.14159265359f / 2 - i * stackStep;
        float xy = radius * cos(stackAngle);
        float z = radius * sin(stackAngle);

        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * sectorStep;
            float x = xy * cos(sectorAngle);
            float y = xy * sin(sectorAngle);
            vec3 pos = vec3(x, y, z);
            vec3 normal = normalize(pos);
            vec3 color = vec3(0.9f, 0.8f, 0.1f);
            vertices.push_back({ pos, color, normal });
        }
    }

    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;
        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    glGenBuffers(1, &VBO_Sphere);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Sphere);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &IBO_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_Sphere);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}


void BindSphere() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_Sphere);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(sizeof(vec3)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(2 * sizeof(vec3)));
    glEnableVertexAttribArray(3);
}

void CompileShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name, GLuint& programId) {
    programId = InitShader(vertex_shader_file_name, fragment_shader_file_name);
    glUseProgram(programId);
}

int Init() {
    GLenum err = glewInit();
    if (err != GLEW_OK) return 1;
    CompileShader("VS.glsl", "FS.glsl", BasicProgramId);
    CreateColoredTriangle();
    CreateCube();
    CreateSphere(0.5f, 32, 32);
    Theta_Location = glGetUniformLocation(BasicProgramId, "theta");
    model_mat_location = glGetUniformLocation(BasicProgramId, "model_mat");
    view_mat_location = glGetUniformLocation(BasicProgramId, "view_mat");
    projection_mat_location = glGetUniformLocation(BasicProgramId, "projection_mat");
    lightPos_location = glGetUniformLocation(BasicProgramId, "lightPos");
    lightColor_location = glGetUniformLocation(BasicProgramId, "lightColor");
    viewPos_location = glGetUniformLocation(BasicProgramId, "viewPos");

    ProjectionMat = perspectiveFov(60.0f, (float)WIDTH, (float)HEIGHT, 0.1f, 100.0f);
    glClearColor(0, 0.2, 0.2, 1);
    glEnable(GL_DEPTH_TEST);
    return 0;
}

float theta = 0;

void Update() {
    float currentFrame = clock() / static_cast<float>(CLOCKS_PER_SEC);
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    float speed = (middleMouseHeld ? 6.0f : baseSpeed) * deltaTime;

    if (resetViewRequested) {
        cameraPos = vec3(0.0f, 0.0f, 3.0f);
        cameraYaw = -90.0f;
        cameraPitch = 0.0f;
        cameraFront = normalize(vec3(0.0f, 0.0f, -1.0f));
        resetViewRequested = false;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) cameraPos += speed * cameraFront;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) cameraPos -= speed * cameraFront;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) cameraPos -= normalize(cross(cameraFront, cameraUp)) * speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) cameraPos += normalize(cross(cameraFront, cameraUp)) * speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) cameraPos += speed * cameraUp;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) cameraPos -= speed * cameraUp;

    ViewMat = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    theta += 0.5f * deltaTime;
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(BasicProgramId);
    glUniform3fv(lightPos_location, 1, value_ptr(lightPos));
    glUniform3fv(lightColor_location, 1, value_ptr(lightColor));
    glUniform3fv(viewPos_location, 1, value_ptr(cameraPos));
    glDisable(GL_DEPTH_TEST);

    mat4 identity = mat4(1.0f);
    mat4 skyView = mat4(mat3(lookAt(vec3(0.0f), cameraFront, cameraUp)));

    glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, value_ptr(identity));
    glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, value_ptr(skyView));
    glUniformMatrix4fv(projection_mat_location, 1, GL_FALSE, value_ptr(ProjectionMat));

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Sky);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glEnable(GL_DEPTH_TEST);

    glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, value_ptr(ViewMat));
    glUniformMatrix4fv(projection_mat_location, 1, GL_FALSE, value_ptr(ProjectionMat));

    switch (Current_DrawingMode) {
    case Points:
        glPointSize(10); glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    case Lines:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case FilledTriangle:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    }

    BindColoredTriangle();
    ModelMat = translate(vec3(-0.8, 0, 0)) * rotate(theta, vec3(0, 0, 1)) * scale(vec3(0.5f));
    glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, value_ptr(ModelMat));
    glDrawArrays(GL_TRIANGLES, 0, 3);

    BindCube();
    ModelMat = translate(vec3(0.8, 0, 0)) * rotate(theta, vec3(0, 1, 0)) * scale(vec3(0.5f));
    glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, value_ptr(ModelMat));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

    BindSphere();
    ModelMat = translate(vec3(0, 0.8, 0)) * rotate(theta, vec3(1, 0, 0)) * scale(vec3(0.5f));
    glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, value_ptr(ModelMat));
    glDrawElements(GL_TRIANGLES, 32 * 32 * 6, GL_UNSIGNED_INT, NULL);
}

int main() {
    sf::ContextSettings context; context.depthBits = 24;
    sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "OpenGL Viewer", sf::Style::Close, context);
    window.setMouseCursorVisible(true);
    if (Init()) return 1;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseMoved) {
                float xpos = static_cast<float>(event.mouseMove.x);
                float ypos = static_cast<float>(event.mouseMove.y);
                if (firstMouse) {
                    lastX = xpos;
                    lastY = ypos;
                    firstMouse = false;
                }
                float xoffset = xpos - lastX;
                float yoffset = lastY - ypos;
                lastX = xpos;
                lastY = ypos;
                float sensitivity = 0.05f;
                xoffset *= sensitivity;
                yoffset *= sensitivity;
                cameraYaw += xoffset;
                cameraPitch += yoffset;
                vec3 direction;
                direction.x = cos(radians(cameraYaw)) * cos(radians(cameraPitch));
                direction.y = sin(radians(cameraPitch));
                direction.z = sin(radians(cameraYaw)) * cos(radians(cameraPitch));
                cameraFront = normalize(direction);
            }
            if (event.type == sf::Event::MouseWheelScrolled) {
                float scrollDelta = event.mouseWheelScroll.delta;
                float scrollSpeed = 2.5f * deltaTime;
                cameraPos += scrollDelta * scrollSpeed * cameraUp;
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Middle) {
                    middleMouseHeld = true;
                    resetViewRequested = true;
                }
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Middle) {
                    middleMouseHeld = false;
                }
            }
        }
        Update();
        Render();
        window.display();
    }
    return 0;
}