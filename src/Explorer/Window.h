#pragma once
#include <string>
#include <functional>
#include <vector>
#include "Render/GL.h"

struct GLFWwindow;

class Window {
public:
	using FrameSizeListener = std::function<void(int, int)>;
	using MouseButtonListener = std::function<void(int, int, int)>;
	using MouseMotionListener = std::function<void(double, double)>;
	using KeyListener = std::function<void(int, int, int, int)>;

	std::vector<FrameSizeListener*> frameSizeListeners;
	std::vector<MouseButtonListener*> mouseButtonListeners;
	std::vector<MouseMotionListener*> mouseMotionListeners;
	std::vector<KeyListener*> keyListeners;

	GLFWwindow* handle;

	Window();
	Window(int, int, const char*);
	~Window();

	void setGL(GL);
	void create();
	void destroy();
	void acquireContext();
	void releaseContext();
	void bufferSwap();

	// Window Properties
	void setTitle(const std::string&);
	int getWidth();
	int getHeight();
	void getFrameSize(int*, int*);

	// Window Functions
	void holdCursor();
	void freeCursor();
	void getMousePosition(double*, double*);
	bool getKey(int);
	
	void setVisible(bool);
	void setCloseFlag(bool);
	bool getCloseFlag();

	// Event Handling
	FrameSizeListener* addFrameSizeListener(FrameSizeListener);
	void removeFrameSizeListener(FrameSizeListener*);

	MouseButtonListener* addMouseButtonListener(MouseButtonListener);
	void removeMouseButtonListener(MouseButtonListener*);

	MouseMotionListener* addMouseMotionListener(MouseMotionListener);
	void removeMouseMotionListener(MouseMotionListener*);

	KeyListener* addKeyListener(KeyListener);
	void removeKeyListener(KeyListener*);

	static void fireEvents();
	static Window* getWindow(GLFWwindow*);

private:
	GL glSetting;

	int width, height;
	float aspect;
	int frameWidth, frameHeight;
	std::string title;
};
