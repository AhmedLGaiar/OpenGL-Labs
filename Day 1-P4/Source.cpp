#include <iostream>
#include <vector>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <cmath>

using namespace std;

const GLint WIDTH = 600, HEIGHT = 600;
bool isDrawing = false;
vector<sf::Vector2f> drawnPoints;

int Init()
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		cout << "Error initializing GLEW";
		getchar();
		return 1;
	}
	cout << "\tUsing glew " << glewGetString(GLEW_VERSION) << endl;
	cout << "\tVendor: " << glGetString(GL_VENDOR) << endl;
	cout << "\tRenderer: " << glGetString(GL_RENDERER) << endl;
	cout << "\tVersion: " << glGetString(GL_VERSION) << endl;
	cout << "\tGLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	glClearColor(1, 1, 1, 1); // White background
	glDisable(GL_DEPTH_TEST);
	return 0;
}

sf::Vector2f screenToOpenGL(int x, int y)
{
	return {
		(2.0f * x) / WIDTH - 1.0f,
		-((2.0f * y) / HEIGHT - 1.0f)
	};
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0, 0, 0); // black
	glLineWidth(2.0f);
	glBegin(GL_LINE_STRIP);
	for (const auto& point : drawnPoints)
		glVertex2f(point.x, point.y);
	glEnd();
}

int main()
{
	sf::ContextSettings context;
	context.depthBits = 24;
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "Hand Drawing with Mouse", sf::Style::Close, context);

	if (Init()) return 1;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::MouseButtonPressed &&
				event.mouseButton.button == sf::Mouse::Left)
			{
				isDrawing = true;
			}
			else if (event.type == sf::Event::MouseButtonReleased &&
				event.mouseButton.button == sf::Mouse::Left)
			{
				isDrawing = false;
			}

			if (event.type == sf::Event::MouseMoved && isDrawing)
			{
				sf::Vector2f glPos = screenToOpenGL(event.mouseMove.x, event.mouseMove.y);
				drawnPoints.push_back(glPos);
			}
		}

		Render();
		window.display();
	}
	return 0;
}
