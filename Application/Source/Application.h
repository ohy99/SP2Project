
#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Application
{
public:
	Application();
	~Application();
	void Init();
	void Run();
	void Exit();
	static bool IsKeyPressed(unsigned short key);

	static StopWatch m_timer;
	static GLFWwindow* m_window;
	static const unsigned char FPS; // FPS of this game
	static const unsigned int frameTime; // time for each frame
private:

	//Declare a window object
	double windowWidth, windowHeight;
};

#endif