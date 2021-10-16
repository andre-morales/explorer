#include "Renderer.hpp"
#include "RendererImpl.h"
#include "Explorer/Explorer.h"
#include "Explorer/ExplorerGUI.h"
#include "Explorer/Game.h"
#include "Explorer/Window.h"
#include "Explorer/Camera.h"
#include "Game/Universe.h"
#include "Game/Planet.h"
#include "Game/Chunk.h"
#include "Game/TextLog.h"
#include "GUI/GUI.h"
#include "Render/GLContext.h"
#include "Render/TextureAtlas.h"
#include "Render/Image.h"
#include "Render/Font.h"
#include "Render/Color.h"
#include "Render/Geometry.h"
#include "Render/Shader.h"
#include "Exception.h"
#include "GL/glew.h"
#include <string>
#include <iostream>

Renderer::Renderer(Explorer& exp) : impl_(new RendererImpl(*this)), impl(*impl_), explorer(exp){
	log("Render", "Building...");
	window = Sh<Window>(1280, 720, "Explorer");
	log("Render", "Built.");
}

Renderer::~Renderer(){

}

void Renderer::init(GL::GLContext gl){
	log("Render", "Initalizing...");
	glVersion = gl;
	window->setGL(gl);
	window->create();
	//window->acquireContext();
	context = Sh<GLContext>(gl, window);

	if(glewInit() != GLEW_OK){
		throw Exception("GLEW initialization failed!");
	}
	log("Render", "Init Ok.");
}

void Renderer::loadResources(){
	log("Render", "Creating textures...");
	loadAtlas("fonts", 7, 9);
	loadAtlas("blocks_atlas", 64, 64);
	loadTexture("title_bg");
	loadTexture("gui_button", true, true);
	loadTexture("gui_panel", true, true);
	loadTexture("gui_panel2", true, true);
	loadTexture("sun", true, true);
	loadTexture("moon", true, true);
	loadTexture("skybox");
	font = mkShared<Font>(getAtlas("fonts"));
}

void Renderer::shutdown(){
	log("Render", "Stopping...");
	textures.clear();
	context.reset();
	window->destroy();
	log("Render", "Shut down!");
}

void Renderer::begin(){
	GLContext& gi = *context;
	glClearColor(0.2, 0.4, 1.0, 1.0);

	if(glVersion >= GL::C1_5) {
		uint32 quadbuffer;
		glGenBuffers(1, &quadbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, quadbuffer);

		byte quaddata[80];
		interleave(quaddata, 80, Geometry::Quad::verts3_tris, 12, Geometry::Quad::uvs, 8);
		glBufferData(GL_ARRAY_BUFFER, 80, quaddata, GL_STATIC_DRAW);
		gi.buffers.emplace("geom_quad", quadbuffer);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	if(glVersion >= GL::C2_0){
		loadShader("world");
	}

	glAlphaFunc(GL_GREATER, 0.5);
	gi.enableColorMaterial();

	// Sets up ambient light
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, vec4f(0).vec);

	// Sun
	glEnable(GL_LIGHT0);
	
	// Moon
	//glEnable(GL_LIGHT1);
	float mDI = 1.0; // Sun diffuse intensity.
	const float moonDiffuseColor[] = {mDI * 0.1, mDI * 0.15, mDI * 0.4, 1.0f};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, moonDiffuseColor);
}

void Renderer::render(){
	GLContext& gi = *context;

	glViewport(0, 0, window->getWidth(), window->getHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1, 1, 1);

	if(explorer.gameInstance){
		Explorer& ex = explorer;
		Game& gin = *ex.gameInstance;
		
		if(gin.planet){
			Planet& pl = *gin.planet;
			impl.makeSun(pl);

			switch(glVersion) {
				case GL::C1_1: impl.drawGame11(gi); break;
				case GL::C1_5: impl.drawGame15(gi); break;
				case GL::C2_0: impl.drawGame20(gi); break;
			}
		}
	}

	impl.renderUI(gi);

	int err = glGetError();
	if(err != 0) std::cerr << "GL Error: " << err << "\n";
}

void Renderer::freeChunk(Chunk* ch){
	if(ch->vbo) {
		//glDeleteBuffers(1, &ch->vbo);
		//ch->vbo = 0;
	}
}

/* --- Resource management section --- */
sh<Texture> Renderer::loadTexture(const std::string& name){
	return loadTexture(name, false, false);
}
sh<Texture> Renderer::loadTexture(const std::string& name, const std::string& file){
	return loadTexture(name, file, false, false);
}
sh<Texture> Renderer::loadTexture(const std::string& name, bool lmin, bool lmag){
	return loadTexture(name, name + ".png", lmin, lmag);
}

sh<Texture> Renderer::loadTexture(const std::string& name, const std::string& file, bool lmin, bool lmag){
	auto path = "res\\" + file;
	log("Renderer", "Loading texture " + name + " from " + path);
	auto texture = Sh<Texture>(Image(path), lmin, lmag);
	textures.map[name] = texture;
	return texture;
}

Shared<TextureAtlas> Renderer::loadAtlas(const char* str, uint32 tw, uint32 th){
	auto path = std::string("res\\") + str + std::string(".png");
	auto name = std::string(str);
	auto texture = mkShared<TextureAtlas>(tw, th);
	texture->set(Image(path));
	textures.emplace(name, texture);
	log("Renderer", "Loading atlas " + name);
	return texture;
}

Shared<Texture> Renderer::getTexture(const char* tn){
	auto tex = textures.find(tn);
	if(tex != textures.end()) return tex->second;
	return nullptr;
}

sh<Shader> Renderer::loadShader (const std::string& name) {
	return loadShader(name, name + ".sh");
}

sh<Shader> Renderer::loadShader(const std::string& name, const std::string& file) {
	auto path = "res\\shaders\\" + file ;
	log("Renderer", "Loading shader " + name + " from " + path);
	auto shader = Sh<Shader>();
	shader->readShaderFile(path);
	shader->compile();
	context->shaders.map[name] = shader;
	return shader;
}

Shared<TextureAtlas> Renderer::getAtlas(const char* tn){
	auto tex = textures.find(tn);
	if(tex != textures.end()){
		sh<TextureAtlas> atlas = std::dynamic_pointer_cast<TextureAtlas>(tex->second);
		if(atlas) return atlas;
	}
	return nullptr;
}

void Renderer::log(const std::string& cl, const std::string& msg){
	explorer.log(cl, msg);
}
