#include "Window.h"
#include "Exception.h"
#include "GLFW/glfw3.h"
#include "ilib/logging.h"

Window::Window()
: Window(640, 480, "Window"){}

Window::Window(int w, int h, const char* t)
: glSetting(GL::V1_1), width(w), height(h), title(t), aspect(w/(float)h){
	posX = -1;
	posY = -1;
}

Window::~Window(){}

void Window::setGL(GL v){
	this->glSetting = v;
}

void Window::create(){
	log("GLC", "Creating window...");
	if(glfwInit() != GLFW_TRUE){
		throw Exception("GLFW initialization failed!");
	}
	log("GLC", "GLFW Ok.");

	// Setup hints
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	setupGLHints();
	log("GLC", "Configured hints.");

	if(fullscreen){
		auto* monitor = glfwGetPrimaryMonitor();
		if(!monitor) goto CREATE_WINDOWED;
		auto* mode = glfwGetVideoMode(monitor);
		if(!mode) goto CREATE_WINDOWED;

		handle = glfwCreateWindow(mode->width, mode->height, title.c_str(), monitor, nullptr);
		goto CONTEXT_OK;
	}
		
	CREATE_WINDOWED:
	handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	CONTEXT_OK:
	if(posX >= 0 && posY >= 0){
		glfwSetWindowPos(handle, posX, posY);
	}

	glfwShowWindow(handle);

	// Set up event listeners
	glfwSetWindowUserPointer(handle, this);
	glfwSetWindowPosCallback(handle, [](GLFWwindow* gwin, int xp, int yp){
		auto& win = *getWindow(gwin);
		win.posX = xp;
		win.posY = yp;
	});
	glfwSetWindowSizeCallback(handle, [](GLFWwindow* gwin, int w, int h){
		auto& win = *getWindow(gwin);
		win.width = w;
		win.height = h;
	});
	glfwSetFramebufferSizeCallback(handle, [](GLFWwindow* gwin, int w, int h){
		Window& win = *getWindow(gwin);
		win.frameWidth = w;
		win.frameHeight = h;
		win.aspect = w/(float)h;
		for(auto& f : win.frameSizeListeners){ (*f)(w, h); }
	});
	glfwSetMouseButtonCallback(handle, [](GLFWwindow* gwin, int btn, int act, int mods){
		Window& win = *getWindow(gwin);
		for(auto& f : win.mouseButtonListeners){ (*f)(btn, act, mods);}
	});
	glfwSetCursorPosCallback(handle, [](GLFWwindow* gwin, double x, double y){
		Window& win = *getWindow(gwin);
		for(auto& f : win.mouseMotionListeners){ (*f)(x, y); }
	});
	glfwSetKeyCallback(handle, [](GLFWwindow* gwin, int key, int scan, int act, int mods){
		Window& win = *getWindow(gwin);
		for(auto& f : win.keyListeners){(*f)(key, scan, act, mods); }
	});
	glfwSetCharCallback(handle, [](GLFWwindow* gwin, unsigned int chr){
		Window& win = *getWindow(gwin);
		for(auto& f : win.charListeners){(*f)(chr); }
	});
	log("GLC", "Finished.");

	acquireContext();
	log("GL", "Context acquired.");
	//glfwSwapInterval(1);
	log("GL", std::string("Version: ") + (char*)glGetString(GL_VERSION));
	log("GL", std::string("Vendor: ") + (char*)glGetString(GL_VENDOR));
	log("GL", std::string("Renderer: ") + (char*)glGetString(GL_RENDERER));
}

void Window::setupGLHints(){
	#ifdef GL_DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	#endif
	//glfwWindowHint(GLFW_SAMPLES, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glSetting.major());
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glSetting.minor());
	if(glSetting.isLegacy()){
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
	} else if(glSetting.isCore()){
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	} else if(glSetting.isCompatibility()){
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	}
}

void Window::destroy(){
	glfwDestroyWindow(handle);
}

void Window::acquireContext(){
	glfwMakeContextCurrent(handle);
}

void Window::bufferSwap(){
	glfwSwapBuffers(handle);
}

void Window::setVSync(bool v) {
	if(v) {
		glfwSwapInterval(1);
	} else {
		glfwSwapInterval(0);
	}
}

void Window::setVisible(bool v){
	if(!handle) return;
	if(v){
		glfwShowWindow(handle);
	} else {
		glfwHideWindow(handle);
	}
}

void Window::setCloseFlag(bool f){
	return glfwSetWindowShouldClose(handle, f);
}

bool Window::getCloseFlag(){
	if(!handle) return false;
	return glfwWindowShouldClose(handle);
}

void Window::setTitle(const std::string& title){
	this->title = title;
	glfwSetWindowTitle(handle, title.c_str());
}

int Window::getWidth(){
	return width;
}

int Window::getHeight(){
	return height;
}

float Window::getAspectRatio(){
	return width / (float)height;
}

void Window::holdCursor(){
	glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::freeCursor(){
	glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::getFrameSize(int* w, int* h){
	glfwGetFramebufferSize(handle, w, h);
}

void Window::getMousePosition(double* x, double* y) const {
	glfwGetCursorPos(handle, x, y);
}

bool Window::getKey(int key){
	return glfwGetKey(handle, key);
}

void Window::fireEvents(){
	glfwPollEvents();
}

void Window::makeFullscreen() {
	fullscreen = true;
	auto monitor = glfwGetPrimaryMonitor();
	if (!monitor) return;
	auto mode = glfwGetVideoMode(monitor);
	if (!mode) return;
	windowedWidth = width;
	windowedHeight = height;
	glfwSetWindowMonitor(handle, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
}

void Window::makeWindowed() {
	glfwSetWindowMonitor(handle, nullptr, posX, posY, windowedWidth, windowedHeight, 0);
	fullscreen = false;
}

Window* Window::getWindow(GLFWwindow* handle){
	return (Window*)glfwGetWindowUserPointer(handle);
}

// Event Listener Add/Remove
#define ADD_FN(FN_NAME, CONTAINER, TYPE)  \
	Window::TYPE* Window::FN_NAME(TYPE listn){ \
		TYPE* listener = new TYPE(listn);   \
		CONTAINER.emplace_back(listener);   \
		return listener;                    \
	} \

#define REMOVE_FN(FN_NAME, CONTAINER, ERROR_MSG) \
	void Window::FN_NAME(void*& listener){ \
		if(!listener) return; \
		for(auto it = CONTAINER.begin(); it != CONTAINER.end(); ++it){ \
			if(*it == listener){\
				CONTAINER.erase(it); \
				delete listener; \
				listener = nullptr; \
				return; \
			} \
		} \
		throw Exception(ERROR_MSG); \
	}

#define ADD_REMOVE_FN(TYPE, CONTAINER, ADD_FN_NAME, REMOVE_FN_NAME, REMOVE_ERROR_MSG) \
	ADD_FN(ADD_FN_NAME, CONTAINER, TYPE); \
	REMOVE_FN(REMOVE_FN_NAME, CONTAINER, REMOVE_ERROR_MSG); \


ADD_REMOVE_FN(FrameSizeListener, frameSizeListeners, addFrameSizeListener,
	removeFrameSizeListener, "Trying to remove a FrameSizeListener that doesn't exist!");

ADD_REMOVE_FN(MouseButtonListener, mouseButtonListeners, addMouseButtonListener,
	removeMouseButtonListener, "Trying to remove a MouseButtonListener that doesn't exist!");

ADD_REMOVE_FN(MouseMotionListener, mouseMotionListeners, addMouseMotionListener,
	removeMouseMotionListener, "Trying to remove a MouseMotionListener that doesn't exist!");

ADD_REMOVE_FN(KeyListener, keyListeners, addKeyListener,
	removeKeyListener, "Trying to remove a KeyListener that doesn't exist!");

ADD_REMOVE_FN(CharListener, charListeners, addCharListener,
	removeCharListener, "Trying to remove a CharListener that doesn't exist!");

