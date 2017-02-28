
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

	static inline double getWindowWidth() { return windowWidth; }
	static inline double getWindowHeight() { return windowHeight; }

	static inline void setWindowSize(int w, int h) { windowWidth = w; windowHeight = h; }
	static inline void setCloseWindow(bool cw) { closeWindow = cw; }

	static StopWatch m_timer;
	static GLFWwindow* m_window;
	static const unsigned char FPS; // FPS of this game
	static const unsigned int frameTime; // time for each frame
private:

	//Declare a window object
	static double windowWidth, windowHeight;
	static bool closeWindow;
};

#endif