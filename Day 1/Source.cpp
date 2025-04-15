#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <cmath>

using namespace std;

const GLint WIDTH = 600, HEIGHT = 600;

int Init()
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		cout << "Error initializing GLEW";
		getchar();
		return 1;
	}
	if (GLEW_VERSION_3_0)
		cout << "Driver supports OpenGL 3.0\nDetails:\n";

	cout << "\tUsing glew " << glewGetString(GLEW_VERSION) << endl;
	cout << "\tVendor: " << glGetString(GL_VENDOR) << endl;
	cout << "\tRenderer: " << glGetString(GL_RENDERER) << endl;
	cout << "\tVersion: " << glGetString(GL_VERSION) << endl;
	cout << "\tGLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	glClearColor(1, 1, 1, 1); // White background
	glEnable(GL_DEPTH_TEST); 
	return 0;
}

void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b)
{
	int i = int(h / 60.0f) % 6;
	float f = (h / 60.0f) - i;
	float p = v * (1.0f - s);
	float q = v * (1.0f - f * s);
	float t = v * (1.0f - (1.0f - f) * s);

	switch (i)
	{
	case 0: r = v; g = t; b = p; break;
	case 1: r = q; g = v; b = p; break;
	case 2: r = p; g = v; b = t; break;
	case 3: r = p; g = q; b = v; break;
	case 4: r = t; g = p; b = v; break;
	case 5: r = v; g = p; b = q; break;
	}
}

void Render(sf::Clock& clock)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float rotationAngle = clock.getElapsedTime().asSeconds() * 30.0f;

	glPushMatrix();
	glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f); // Rotate 

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1, 1, 1); // Center white
	glVertex2f(0.0f, 0.0f);

	for (int i = 0; i <= 360; ++i)
	{
		float angleRad = i * 3.14159f / 180.0f;
		float r, g, b;
		HSVtoRGB((float)i, 1.0f, 1.0f, r, g, b); 
		glColor3f(r, g, b);
		glVertex2f(cos(angleRad) * 0.8f, sin(angleRad) * 0.8f);
	}
	glEnd();

	glPopMatrix();
}

void Update() {}

int main()
{
	sf::ContextSettings context;
	context.depthBits = 24;
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "Smooth Colorful Wheel", sf::Style::Close, context);

	if (Init()) return 1;

	sf::Clock clock;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		Update();
		Render(clock);
		window.display();
	}
	return 0;
}
