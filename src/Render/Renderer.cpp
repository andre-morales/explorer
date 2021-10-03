#include "Renderer.hpp"
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
#include "Exception.h"
#include "GL/glew.h"
#include <string>
#include <iostream>
#include <sstream>

Renderer::Renderer(Explorer& exp) : explorer(exp){
	log("Render", "Building...");
	window = mkShared<Window>(1280, 720, "Explorer");
	log("Render", "Built.");
}

Renderer::~Renderer(){

}

void Renderer::init(GL gl){
	log("Render", "Initalizing...");
	window->setGL(gl);
	window->create();
	window->acquireContext();
	context = mkShared<GLContext>(window);

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
    loadTexture("skybox");
    font = mkShared<Font>(getAtlas("fonts"));
}

void Renderer::begin(){
	glClearColor(0.2, 0.4, 1.0, 1.0);
	glAlphaFunc(GL_GREATER, 0.5);
}

void Renderer::shutdown(){
	log("Render", "Stopping...");
	textures.clear();
	context.reset();
	window->destroy();
	log("Render", "Shut down!");
}

void Renderer::render(){
	GLContext& gi = *context;

	glViewport(0, 0, window->getWidth(), window->getHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(explorer.gameInstance){
        drawGame11();
	}

	gi.disableFaceCulling();
	gi.disableBlending();
	gi.disableDepthTesting();
	gi.enableAlphaTesting();

	glMatrixMode(GL_PROJECTION);
	float wh = window->getHeight();
	float hw = window->getWidth() / 2.0f;
	float hh = window->getHeight() / 2.0f;
	glLoadMatrixf(mat4<float>::ortho(-hw, hw, hh, -hh, -1, 1));
	glTranslatef(-hw, -hh, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    explorer.ui->render();

    int err = glGetError();
	if(err != 0) std::cerr << "GL Error: " << err << "\n";
}

void Renderer::drawGame11(){
	glColor3f(1, 1, 1);

	Explorer& ex = explorer;
	GLContext& gi = *context;
    Game& gin = *ex.gameInstance;
    Camera& cam = *gin.camera;
	//Player& mp = *gin.player;

	cam.aspect = window->getWidth() / (float)window->getHeight();
	cam.makeView();
	cam.makeTransform();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(cam.makeProjection());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(cam.rot.x, 1, 0, 0);
	glRotatef(cam.rot.y, 0, 1, 0);

	if(!gin.universe) return;
	Universe& uni = *gin.universe;
	if(uni.planets.size() == 0) return;
	Planet& pl = *(uni.planets[0]);

	// Skybox
	gi.enableTexture2d();
	gi.enableVertsArray();
	gi.enableUVsArray();
	gi.disableDepthTesting();

    bindTexture("skybox");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glVertexPointer(3, GL_FLOAT, 0, skybox_verts);
    glTexCoordPointer(2, GL_FLOAT, 0, skybox_uvs);
    glDrawArrays(GL_QUADS, 0, 24);

    gi.enableTexture2d();
	gi.enableFaceCulling();
	gi.enableDepthTesting();
    gi.disableAlphaTesting();
	gi.setColorsArray(chunkColors);
	bindTexture("blocks_atlas");

	// Render Slices
	for(Chunk* chunk_ : pl.chunkSet){
		Chunk& chunk = *chunk_;

		if(chunk.batchedCorners){
			double _x = chunk.cx * 24, _y = chunk.cy * 24, _z = chunk.cz * 24;
			glLoadMatrixd(mat4d::translation(_x, _y, _z) * cam.view);
			glVertexPointer(3, GL_FLOAT, 0, chunk.batchedVerts);
			glTexCoordPointer(2, GL_FLOAT, 0, chunk.batchedUVs);

			//glNormalPointer(GL_BYTE, 0, chunk.batchedNormals);
			if(chunkColors)glColorPointer(3, GL_UNSIGNED_BYTE, 0, chunk.batchedColors);
			glDrawArrays(GL_QUADS, 0, chunk.batchedVertsLen);
		}
	}

	gi.disableColorsArray();
}

Shared<Texture> Renderer::loadTexture(const char* str){
	return loadTexture(str, false, false);
}

Shared<Texture> Renderer::loadTexture(const char* str, bool lmin, bool lmag){
	auto path = std::string("res\\") + str + std::string(".png");
	auto name = std::string(str);
	auto texture = mkShared<Texture>(Image(path), lmin, lmag);
	textures.emplace(name, texture);
	log("Renderer", "Loading texture " + name);
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

void Renderer::bindTexture(const char* tn){
	auto tex = textures.find(tn);
	if(tex != textures.end()) tex->second->bind();
}

Shared<Texture> Renderer::getTexture(const char* tn){
	auto tex = textures.find(tn);
	if(tex != textures.end()) return tex->second;
	return nullptr;
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
