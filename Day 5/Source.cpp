#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include<gl\glm\glm.hpp>
#include<gl\glm\gtc\type_ptr.hpp>
#include<gl\glm\gtx\transform.hpp>
#include <ctime> // For clock() function

using namespace std;
using namespace glm;

enum DrawingMode
{
	Points,
	Lines,
	FilledTriangle
};

struct Vertex
{
	vec3 Position;
	vec3 Color;
	vec2 UV;

	Vertex() {}
	Vertex(vec3 _position) :Position(_position), Color(_position) {}
	Vertex(vec3 _position, vec3 _color) :Position(_position), Color(_color) {}
	Vertex(vec3 _position, vec3 _color, vec2 _uv) :Position(_position), Color(_color), UV(_uv) {}
};

const GLint WIDTH = 600, HEIGHT = 600;
DrawingMode Current_DrawingMode = DrawingMode::FilledTriangle;
GLuint BasicProgramId, LightsProgramId, LightsPhongProgramId, TextureProgramId, SkyboxProgramId, ReflectProgramId, RefractProgramId;
GLuint VBO_Triangle, VBO_ColoredTriangle, VBO_Cube, VBO_Sphere;
GLuint IBO_Cube;
GLuint TextureId, SkyboxTextureId;
GLuint Theta_Location, model_mat_location, view_mat_location, projection_mat_location;
mat4 ModelMat, ViewMat, ProjectionMat;

// Camera variables from first code
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

GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name);

#pragma region sphere
vector<Vertex> sphere_vertices;

void Triangle(vec3 a, vec3 b, vec3 c)
{
	vec3 FaceNormal = normalize(cross((b - a), (c - a)));
	sphere_vertices.push_back(Vertex(a, a));
	sphere_vertices.push_back(Vertex(b, b));
	sphere_vertices.push_back(Vertex(c, c));
}

void DividTriangle(vec3 a, vec3 b, vec3 c, int iterations)
{
	if (iterations == 0) // terminator
	{
		Triangle(a, b, c);
		return;
	}

	vec3 v1 = normalize(a + b);
	vec3 v2 = normalize(a + c);
	vec3 v3 = normalize(b + c);

	// divide more
	DividTriangle(a, v1, v2, iterations - 1);
	DividTriangle(v1, b, v3, iterations - 1);
	DividTriangle(v1, v3, v2, iterations - 1);
	DividTriangle(v2, v3, c, iterations - 1);
}

void CreateSphere(int iterations)
{
	vec3 Sphere_Core_vertices[4] =
	{
		vec3(0.0, 0.0, 1.0), // 0
		vec3(0.0, 0.942809, -0.333333),  // 1
		vec3(-0.816497, -0.471405, -0.333333),  // 2
		vec3(0.816497, -0.471405, -0.333333) // 3
	};

	sphere_vertices.clear();

	DividTriangle(Sphere_Core_vertices[0], Sphere_Core_vertices[1], Sphere_Core_vertices[2], iterations);
	DividTriangle(Sphere_Core_vertices[0], Sphere_Core_vertices[3], Sphere_Core_vertices[1], iterations);
	DividTriangle(Sphere_Core_vertices[0], Sphere_Core_vertices[2], Sphere_Core_vertices[3], iterations);
	DividTriangle(Sphere_Core_vertices[3], Sphere_Core_vertices[2], Sphere_Core_vertices[1], iterations);

	// generate buffer id
	glGenBuffers(1, &VBO_Sphere);

	// bind buffer by id
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Sphere);

	// assign buffer data
	glBufferData(GL_ARRAY_BUFFER, sphere_vertices.size() * sizeof(Vertex), sphere_vertices.data(), GL_STATIC_DRAW);

	// handle shaders
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(2);
}

void BindSphere()
{
	// bind buffer by id
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Sphere);

	// handle shaders
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(2);
}
#pragma endregion

void CreateTriangle()
{
	GLfloat TriangleVertices[] =
	{
		-1,-1,0,
		1,-1,0,
		0,1,0,
		-0.5,-0.5,0,
		0.5,-0.5,0,
		0,0.5,0
	};

	// generate buffer id
	glGenBuffers(1, &VBO_Triangle);

	// bind buffer by id
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Triangle);

	// assign buffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleVertices), TriangleVertices, GL_STATIC_DRAW);

	// handle shaders
	//GLuint Vertex_Position_Location = glGetAttribLocation(BasicProgramId, "vertex_position");
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(1);
}

void BindTriangle()
{
	// bind buffer by id
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Triangle);

	// handle shaders
	//GLuint Vertex_Position_Location = glGetAttribLocation(BasicProgramId, "vertex_position");
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(1);
}

void CreateColoredTriangle()
{
	GLfloat TriangleVertices[] =
	{
		-1,-1,0,0,1,0,
		1,-1,0,0,0,1,
		0,1,0,1,0,0
	};

	// generate buffer id
	glGenBuffers(1, &VBO_ColoredTriangle);

	// bind buffer by id
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ColoredTriangle);

	// assign buffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleVertices), TriangleVertices, GL_STATIC_DRAW);

	// handle shaders
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(GL_FLOAT), 0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, false, 6 * sizeof(GL_FLOAT), (char*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
}

void BindColoredTriangle()
{
	// bind buffer by id
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ColoredTriangle);

	// handle shaders
	// position
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(GL_FLOAT), 0);
	glEnableVertexAttribArray(1);

	// color
	glVertexAttribPointer(2, 3, GL_FLOAT, false, 6 * sizeof(GL_FLOAT), (char*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
}

void CreateCube()
{
	// Modified cube vertices with two color variations
	Vertex cube_core_vertices[] = {
	{vec3(-0.5, 0.5, 0.5),vec3(0,0,1),vec2(0,1)},  // Blue color (0,0,1)
	{vec3(-0.5, -0.5, 0.5),vec3(0,0,1),vec2(0,0)}, // Blue color
	{vec3(0.5, -0.5, 0.5),vec3(0,0,1),vec2(1,0)},  // Blue color
	{vec3(0.5, 0.5, 0.5),vec3(0,0,1),vec2(1,1)},   // Blue color
	{vec3(0.5, 0.5, -0.5),vec3(0,0,1), vec2(1,1)}, // Blue color
	{vec3(0.5, -0.5, -0.5),vec3(0,0,1), vec2(1,1)},// Blue color
	{vec3(-0.5, -0.5, -0.5),vec3(0,0,1), vec2(1,1)},// Blue color
	{vec3(-0.5, 0.5, -0.5),vec3(0,0,1), vec2(1,1)} // Blue color
	};

	int vertices_Indeces[] = {
		//front
		0,1,2,
		0,2,3,
		//Right
		3,2,5,
		3,5,4,
		//Back
		4,5,6,
		4,6,7,
		//Left
		7,6,1,
		7,1,0,
		//Top
		7,0,3,
		7,3,4,
		//Bottom
		2,1,6,
		2,6,5
	};

	// generate buffer id
	glGenBuffers(1, &VBO_Cube);

	// bind buffer by id
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);

	// assign buffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_core_vertices), cube_core_vertices, GL_STATIC_DRAW);

	// index buffer

	// generate index buffer id
	glGenBuffers(1, &IBO_Cube);

	// bind index buffer by id
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_Cube);

	// assign index buffer data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices_Indeces), vertices_Indeces, GL_STATIC_DRAW);

	// handle shaders
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 2, GL_FLOAT, false, sizeof(Vertex), (char*)(2 * sizeof(vec3)));
	glEnableVertexAttribArray(3);
}

void BindCube()
{
	// bind buffer by id
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);

	// bind index buffer by id
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_Cube);

	// handle shaders
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 2, GL_FLOAT, false, sizeof(Vertex), (char*)(2 * sizeof(vec3)));
	glEnableVertexAttribArray(3);
}

void CompileShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name, GLuint& programId)
{
	programId = InitShader(vertex_shader_file_name, fragment_shader_file_name);
}

void UseShader(GLuint programId)
{
	glUseProgram(programId);

	Theta_Location = glGetUniformLocation(programId, "theta");
	model_mat_location = glGetUniformLocation(programId, "model_mat");
	view_mat_location = glGetUniformLocation(programId, "view_mat");
	projection_mat_location = glGetUniformLocation(programId, "projection_mat");

	// Now we update the view matrix using our camera system
	ViewMat = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, value_ptr(ViewMat));

	ProjectionMat = perspectiveFov(60.0f, (float)WIDTH, (float)HEIGHT, 0.1f, 100.0f);
	glUniformMatrix4fv(projection_mat_location, 1, GL_FALSE, value_ptr(ProjectionMat));
}

void LoadImage()
{
	sf::Image img;
	if (!img.loadFromFile("UV_Grid.jpg"))
	{
		cout << "Error Loading Image";
		return;
	}

	// generate buffer id
	glGenTextures(1, &TextureId);

	// bind buffer by id
	glBindTexture(GL_TEXTURE_2D, TextureId);

	// assign buffer data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());

	// assign texture params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

void LoadSkyBox()
{
	string cub_maps[6] =
	{
		"skybox\\rt.png",
		"skybox\\lf.png",
		"skybox\\up.png",
		"skybox\\dn.png",
		"skybox\\bk.png",
		"skybox\\ft.png"
	};

	// generate buffer id
	glGenTextures(1, &SkyboxTextureId);

	// bind buffer by id
	glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxTextureId);

	// assign buffer data
	sf::Image img;
	for (int i = 0; i < 6; i++)
	{
		if (!img.loadFromFile(cub_maps[i]))
		{
			cout << "Can't Load Image: " << cub_maps[i] << endl;
			continue;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
}

int Init()
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		cout << "Error";
		getchar();
		return 1;
	}
	else
	{
		if (GLEW_VERSION_3_0)
			cout << "Driver support OpenGL 3.0\nDetails:\n";
	}
	cout << "\tUsing glew " << glewGetString(GLEW_VERSION) << endl;
	cout << "\tVendor: " << glGetString(GL_VENDOR) << endl;
	cout << "\tRenderer: " << glGetString(GL_RENDERER) << endl;
	cout << "\tVersion: " << glGetString(GL_VERSION) << endl;
	cout << "\tGLSL:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	CompileShader("VS.glsl", "FS.glsl", BasicProgramId);
	CompileShader("VS_light.glsl", "FS_light.glsl", LightsProgramId);
	CompileShader("VS_lightPhong.glsl", "FS_lightPhong.glsl", LightsPhongProgramId);
	CompileShader("VS_Texture.glsl", "FS_Texture.glsl", TextureProgramId);
	CompileShader("VS_Skybox.glsl", "FS_Skybox.glsl", SkyboxProgramId);
	CompileShader("VS_Reflection.glsl", "FS_Reflection.glsl", ReflectProgramId);
	CompileShader("VS_Refraction.glsl", "FS_Refraction.glsl", RefractProgramId);

	CreateColoredTriangle();
	CreateCube();
	CreateSphere(5);

	LoadImage();
	LoadSkyBox();

	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);

	return 0;
}

float theta = 0;
void Update()
{
	// Calculate delta time
	float currentFrame = clock() / static_cast<float>(CLOCKS_PER_SEC);
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	float speed = (middleMouseHeld ? 6.0f : baseSpeed) * deltaTime;

	// Reset view if requested
	if (resetViewRequested) {
		cameraPos = vec3(0.0f, 0.0f, 3.0f);
		cameraYaw = -90.0f;
		cameraPitch = 0.0f;
		cameraFront = normalize(vec3(0.0f, 0.0f, -1.0f));
		resetViewRequested = false;
	}

	// Process keyboard input for camera movement
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) cameraPos += speed * cameraFront;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) cameraPos -= speed * cameraFront;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) cameraPos -= normalize(cross(cameraFront, cameraUp)) * speed;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) cameraPos += normalize(cross(cameraFront, cameraUp)) * speed;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) cameraPos += speed * cameraUp;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) cameraPos -= speed * cameraUp;

	// Update the view matrix based on camera position
	ViewMat = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	theta += 0.001f;
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// skybox
	glDepthMask(GL_FALSE);
	BindCube();
	UseShader(SkyboxProgramId);
	ModelMat = translate(vec3(0, 0, 0)) *
		rotate(-theta * 180 / 3.14f, vec3(0, 1, 0)) *
		scale(vec3(50, 50, 50));

	glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, value_ptr(ModelMat));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
	glDepthMask(GL_TRUE);

	// environment
	// draw
	switch (Current_DrawingMode)
	{
	case Points:
		glPointSize(10);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case Lines:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case FilledTriangle:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	default:
		break;
	}

	// Sphere at the top (reflective)
	BindSphere();
	UseShader(ReflectProgramId);  // Keep using ReflectProgramId for the sphere
	ModelMat = translate(vec3(0, 1.0, 0)) *
		rotate(theta * 180 / 3.14f, vec3(0, 1, 0)) *
		scale(vec3(0.6, 0.6, 0.6));

	glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, value_ptr(ModelMat));
	glDrawArrays(GL_TRIANGLES, 0, sphere_vertices.size());

	// Left cube at the bottom (reflective)
	BindCube();
	UseShader(ReflectProgramId);  // Left cube also using ReflectProgramId
	ModelMat = translate(vec3(-1.0, -0.8, 0)) *
		rotate(theta * 180 / 3.14f, vec3(0, 1, 0)) *
		scale(vec3(0.5, 0.5, 0.5));

	glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, value_ptr(ModelMat));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

	// Right cube at the bottom (refractive)
	BindCube();
	UseShader(RefractProgramId);  // Right cube using RefractProgramId
	ModelMat = translate(vec3(1.0, -0.8, 0)) *
		rotate(theta * 180 / 3.14f, vec3(0, 1, 0)) *
		scale(vec3(0.5, 0.5, 0.5));

	glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, value_ptr(ModelMat));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
}

int main()
{
	sf::ContextSettings context;
	context.depthBits = 24;
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "One Sphere and Two Cubes with FPS Camera", sf::Style::Close, context);

	// Center the mouse cursor initially
	sf::Mouse::setPosition(sf::Vector2i(WIDTH / 2, HEIGHT / 2), window);
	window.setMouseCursorVisible(false); // Hide cursor for better FPS camera experience

	if (Init()) return 1;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;

			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Num1)
				{
					Current_DrawingMode = DrawingMode::Points;
				}
				if (event.key.code == sf::Keyboard::Num2)
				{
					Current_DrawingMode = DrawingMode::Lines;
				}
				if (event.key.code == sf::Keyboard::Num3)
				{
					Current_DrawingMode = DrawingMode::FilledTriangle;
				}
				if (event.key.code == sf::Keyboard::R)
				{
					resetViewRequested = true;
				}
				if (event.key.code == sf::Keyboard::Escape)
				{
					window.close();
				}
				break;

			case sf::Event::MouseMoved:
			{
				float xpos = static_cast<float>(event.mouseMove.x);
				float ypos = static_cast<float>(event.mouseMove.y);

				if (firstMouse) {
					lastX = xpos;
					lastY = ypos;
					firstMouse = false;
				}

				float xoffset = xpos - lastX;
				float yoffset = lastY - ypos; // Reversed: y ranges bottom to top
				lastX = xpos;
				lastY = ypos;

				float sensitivity = 0.05f;
				xoffset *= sensitivity;
				yoffset *= sensitivity;

				cameraYaw += xoffset;
				cameraPitch += yoffset;

				// Constrain pitch to avoid gimbal lock
				if (cameraPitch > 89.0f)
					cameraPitch = 89.0f;
				if (cameraPitch < -89.0f)
					cameraPitch = -89.0f;

				// Calculate new front vector
				vec3 direction;
				direction.x = cos(radians(cameraYaw)) * cos(radians(cameraPitch));
				direction.y = sin(radians(cameraPitch));
				direction.z = sin(radians(cameraYaw)) * cos(radians(cameraPitch));
				cameraFront = normalize(direction);

				// Reset mouse position to center of screen to prevent edge hitting
				sf::Mouse::setPosition(sf::Vector2i(WIDTH / 2, HEIGHT / 2), window);
				lastX = WIDTH / 2;
				lastY = HEIGHT / 2;
			}
			break;

			case sf::Event::MouseWheelScrolled:
			{
				float scrollDelta = event.mouseWheelScroll.delta;
				float scrollSpeed = 2.5f * deltaTime;
				baseSpeed += scrollDelta * 0.2f; // Adjust movement speed with scroll wheel
				if (baseSpeed < 0.5f) baseSpeed = 0.5f; // Minimum speed
				if (baseSpeed > 10.0f) baseSpeed = 10.0f; // Maximum speed
			}
			break;

			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Middle) {
					middleMouseHeld = true;
					resetViewRequested = true;
				}
				break;

			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Middle) {
					middleMouseHeld = false;
				}
				break;
			}
		}

		Update();
		Render();

		window.display();
	}
	return 0;
}