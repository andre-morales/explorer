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
	using CharListener = std::function<void(unsigned int)>;

	std::vector<FrameSizeListener*> frameSizeListeners;
	std::vector<MouseButtonListener*> mouseButtonListeners;
	std::vector<MouseMotionListener*> mouseMotionListeners;
	std::vector<KeyListener*> keyListeners;
	std::vector<CharListener*> charListeners;

	GL glSetting;

	Window();
	Window(int, int, const char*);
	~Window();

	void setGL(GL);
	void create();
	void destroy();
	void acquireContext();
	void releaseContext();
	void bufferSwap();
	void setVSync(bool);

	// Window Properties
	void setTitle(const std::string&);
	int getWidth();
	int getHeight();
	float getAspectRatio();
	void getFrameSize(int*, int*);

	// Window Functions
	void holdCursor();
	void freeCursor();
	void getMousePosition(double*, double*) const;
	bool getKey(int);
	
	void setVisible(bool);
	void setCloseFlag(bool);
	bool getCloseFlag();

	void makeFullscreen();
	void makeWindowed();

	// Event Handling
	FrameSizeListener* addFrameSizeListener(FrameSizeListener);
	void removeFrameSizeListener(void*&);

	MouseButtonListener* addMouseButtonListener(MouseButtonListener);
	void removeMouseButtonListener(void*&);

	MouseMotionListener* addMouseMotionListener(MouseMotionListener);
	void removeMouseMotionListener(void*&);

	KeyListener* addKeyListener(KeyListener);
	void removeKeyListener(void*&);

	CharListener* addCharListener(CharListener);
	void removeCharListener(void*&);

	static void fireEvents();
	static Window* getWindow(GLFWwindow*);

private:
	GLFWwindow* handle = 0;
	

	bool fullscreen = false;
	int posX, posY;
	int width, height;
	int windowedWidth = 0, windowedHeight = 0;
	int frameWidth = 0, frameHeight = 0;
	float aspect;
	std::string title;

	void setupGLHints();
};
