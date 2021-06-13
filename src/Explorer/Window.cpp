#include "Window.h"
#include "Exception.h"
#include "GLFW/glfw3.h"

Window::Window()
: Window(640, 480, "Window"){}

Window::Window(int w, int h, const char* t)
: width(w), height(h), title(t), glSetting(GL::V1_1){}

Window::~Window(){

}

void Window::setGL(GL v){
	this->glSetting = v;
}

void Window::create(){
	if(glfwInit() != GLFW_TRUE){
		throw Exception("GLFW initialization failed!");
	}

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
	handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	// Set up event listeners
	glfwSetWindowUserPointer(handle, this);
	glfwSetFramebufferSizeCallback(handle, [](GLFWwindow* gwin, int w, int h){
		Window& win = *getWindow(gwin);
		win.frameWidth = w;
		win.frameHeight = h;
		win.aspect = w/(float)h;
		for(auto& f : win.frameSizeListeners){ (*f)(w, h); }
	});
	glfwSetMouseButtonCallback(handle, [](GLFWwindow* gwin, int btn, int act, int mods){
		Window& win = *getWindow(gwin);
		for(auto& f : win.mouseButtonListeners){ (*f)(btn, act, mods); }
	});
	glfwSetCursorPosCallback(handle, [](GLFWwindow* gwin, double x, double y){
		Window& win = *getWindow(gwin);
		for(auto& f : win.mouseMotionListeners){ (*f)(x, y); }
	});
	glfwSetKeyCallback(handle, [](GLFWwindow* gwin, int key, int scan, int act, int mods){
		Window& win = *getWindow(gwin);
		for(auto& f : win.keyListeners){(*f)(key, scan, act, mods); }
	});
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
	int w;
	glfwGetWindowSize(handle, &w, nullptr);
	return w;
}

int Window::getHeight(){
	int h;
	glfwGetWindowSize(handle, nullptr, &h);
	return h;
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

void Window::getMousePosition(double* x, double* y){
	glfwGetCursorPos(handle, x, y);
}

bool Window::getKey(int key){
	return glfwGetKey(handle, key);
}

void Window::fireEvents(){
	glfwPollEvents();
}

Window::FrameSizeListener* Window::addFrameSizeListener(FrameSizeListener listener){
	FrameSizeListener* fsl = new FrameSizeListener(listener);
	frameSizeListeners.emplace_back(fsl);
	return fsl;
}
void Window::removeFrameSizeListener(FrameSizeListener* listener){
	for(auto it = frameSizeListeners.begin(); it != frameSizeListeners.end(); ++it){
		if(*it == listener){
			frameSizeListeners.erase(it);
			return;
		}
	}
	throw Exception("Trying to remove a FrameSizeListener that doesn't exist!");
}

Window::MouseButtonListener* Window::addMouseButtonListener(MouseButtonListener listener){
	MouseButtonListener* mbl = new MouseButtonListener(listener);
	mouseButtonListeners.emplace_back(mbl);
	return mbl;
}
void Window::removeMouseButtonListener(MouseButtonListener* listener){
	for(auto it = mouseButtonListeners.begin(); it != mouseButtonListeners.end(); ++it){
		if(*it == listener){
			mouseButtonListeners.erase(it);
			return;
		}
	}
	throw Exception("Trying to remove a MouseButtonListener that doesn't exist!");
}

Window::MouseMotionListener* Window::addMouseMotionListener(MouseMotionListener listener){
	MouseMotionListener* mml = new MouseMotionListener(listener);
	mouseMotionListeners.emplace_back(mml);
	return mml;
}
void Window::removeMouseMotionListener(MouseMotionListener* listener){
	for(auto it = mouseMotionListeners.begin(); it != mouseMotionListeners.end(); ++it){
		if(*it == listener){
			mouseMotionListeners.erase(it);
			return;
		}
	}
	throw Exception("Trying to remove a MouseMotionListener that doesn't exist!");
}

Window::KeyListener* Window::addKeyListener(KeyListener listener){
	KeyListener* kl = new KeyListener(listener);
	keyListeners.emplace_back(kl);
	return kl;
}
void Window::removeKeyListener(KeyListener* listener){
	for(auto it = keyListeners.begin(); it != keyListeners.end(); ++it){
		if(*it == listener){
			keyListeners.erase(it);
			return;
		}
	}
	throw Exception("Trying to remove a KeyListener that doesn't exist!");
}

Window* Window::getWindow(GLFWwindow* handle){
	return (Window*)glfwGetWindowUserPointer(handle);
}
