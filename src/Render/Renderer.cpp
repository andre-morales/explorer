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
#include "Render/Geometry.h"
#include "Exception.h"
#include "GL/glew.h"
#include <string>
#include <iostream>

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
	glAlphaFunc(GL_GREATER, 0.5);
	gi.enableColorMaterial();

	// Sets up ambient light
	const float ambientLight[] = {0.6f, 0.6f, 0.6f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	// Sun
	glEnable(GL_LIGHT0);
	const float sunDiffuseColor[] = {0.4 * 1.0, 0.4 * 0.9, 0.4 * 0.8, 1.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiffuseColor);
	
	// Sun
	glEnable(GL_LIGHT1);
	const float moonDiffuseColor[] = {0.0, 0.1, 0.5, 1.0f};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, moonDiffuseColor);
}

void Renderer::render(){
	GLContext& gi = *context;

	glViewport(0, 0, window->getWidth(), window->getHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1, 1, 1);

	if(explorer.gameInstance){
        drawGame11(gi);
	}

    renderUI(gi);

    int err = glGetError();
	if(err != 0) std::cerr << "GL Error: " << err << "\n";
}

void Renderer::renderUI(GLContext& gi){
	gi.disableFaceCulling();
	gi.disableBlending();
	gi.disableDepthTesting();
	gi.enableAlphaTesting();

	glMatrixMode(GL_PROJECTION);
	float hw = gi.getWidth() / 2.0f;
	float hh = gi.getHeight() / 2.0f;
	glLoadMatrixf(mat4f::ortho(-hw, hw, hh, -hh, -1, 1));
	glTranslatef(-hw, -hh, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	explorer.ui->render();
}

void Renderer::drawGame11(GLContext& gi){
	Explorer& ex = explorer;
    Game& gin = *ex.gameInstance;

	if(!gin.universe) return;
	Universe& uni = *gin.universe;
	if(uni.planets.size() == 0) return;
	Planet& pl = *(uni.planets[0]);

	// Setup projection
	Camera& cam = *gin.camera;
	cam.aspect = window->getAspectRatio();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(cam.makeProjection());

	// Setup just camera roation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(cam.rot.x, 1, 0, 0);
	glRotatef(cam.rot.y, 0, 1, 0);

	// Skybox
	gi.enableTexture2d();
	gi.enableVertsArray();
	gi.enableUVsArray();
	gi.disableDepthTesting();
	gi.disableFaceCulling();

    bindTexture("skybox");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glVertexPointer(3, GL_FLOAT, 0, skybox_verts);
    glTexCoordPointer(2, GL_FLOAT, 0, skybox_uvs);
    glDrawArrays(GL_QUADS, 0, 24);

	// Sun
	gi.enableBlending();
	bindTexture("sun");
	glPushMatrix();
	glRotatef(-pl.sunXRotation, 0, 1, 0);
	glRotatef(pl.dayTime * 360 - 90, 1, 0, 0);
	glScalef(0.42, 0.42, 1);
	glTranslatef(0, 0, -1);
	glVertexPointer(3, GL_FLOAT, 0, Quad::verts3_tris);
	glTexCoordPointer(2, GL_FLOAT, 0, Quad::uvs);
	glDrawArrays(GL_QUADS, 0, 4);
	glPopMatrix();

	// Moon
	bindTexture("moon");
	glRotatef(-pl.sunXRotation, 0, 1, 0);
	glRotatef(pl.dayTime * 360 + 90, 1, 0, 0);
	glScalef(0.42, 0.42, 1);
	glTranslatef(0, 0, -1);
	glVertexPointer(3, GL_FLOAT, 0, Quad::verts3_tris);
	glTexCoordPointer(2, GL_FLOAT, 0, Quad::uvs);
	glDrawArrays(GL_QUADS, 0, 4);

	// Render chunks
	glLoadMatrixd(cam.makeView());
	float sunAngle = pl.dayTime * 360;

	const float sunpos[] = {sindf(sunAngle) * sindf(pl.sunXRotation), sindf(sunAngle - 90), sindf(sunAngle) * cosdf(pl.sunXRotation), 0};
	glLightfv(GL_LIGHT0, GL_POSITION, sunpos);

	const float moonpos[] = {sindf(sunAngle + 180) * sindf(pl.sunXRotation), sindf(sunAngle + 90), sindf(sunAngle  + 180) * cosdf(pl.sunXRotation), 0};
	glLightfv(GL_LIGHT1, GL_POSITION, moonpos);

	gi.enableVertsArray();
	gi.enableUVsArray();
	gi.enableFaceCulling();
	gi.enableDepthTesting();
    gi.disableAlphaTesting();
	gi.disableBlending();

	gi.setColorsArray(chunkColors);
	gi.enableLighting();
	gi.enableNormalsArray();
	
	bindTexture("blocks_atlas");
	
	
	for(Chunk* chunk_ : pl.chunkSet){
		Chunk& chunk = *chunk_;

		if(chunk.batchedCorners){
			double _x = chunk.cx * 24, _y = chunk.cy * 24, _z = chunk.cz * 24;
			glLoadMatrixd(mat4d::translation(_x, _y, _z) * cam.view);
			glVertexPointer(3, GL_FLOAT, 0, chunk.batchedVerts);
			glTexCoordPointer(2, GL_FLOAT, 0, chunk.batchedUVs);

			glNormalPointer(GL_FLOAT, 0, chunk.batchedNormals);
			if(chunkColors) glColorPointer(3, GL_UNSIGNED_BYTE, 0, chunk.batchedColors);
			glDrawArrays(GL_QUADS, 0, chunk.batchedVertsLen);
		} else if(chunk.batchedCenter){
			gi.disableTexture2d();
			double _x = chunk.cx * 24, _y = chunk.cy * 24, _z = chunk.cz * 24;
			glLoadMatrixd(mat4d::translation(_x, _y, _z) * cam.view);
			glVertexPointer(3, GL_FLOAT, 0, chunk.batchedVerts);
			//glTexCoordPointer(2, GL_FLOAT, 0, chunk.batchedUVs);

			glNormalPointer(GL_FLOAT, 0, chunk.batchedNormals);
			glColorPointer(3, GL_UNSIGNED_BYTE, 0, chunk.batchedColors);
			glDrawArrays(GL_QUADS, 0, chunk.batchedVertsLen);
			gi.enableTexture2d();
		} else if(chunk.blocks){
			glColor3f(0.8, 0.1, 1.0);
			gi.disableTexture2d();
			gi.disableLighting();
			double _x = chunk.cx * 24, _y = chunk.cy * 24, _z = chunk.cz * 24;
			glLoadMatrixd(mat4d::translation(_x, _y, _z) * cam.view);
			glBegin(GL_QUADS);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 0, 24);
			glVertex3f(24, 0, 24);
			glVertex3f(24, 0, 0);
			glEnd();
			gi.enableTexture2d();
			gi.enableLighting();
		} else if(chunk.state == Chunk::State::REQUESTED){
			glColor3f(0.1, 0.1, 1.0);
			gi.disableTexture2d();
			gi.disableLighting();
			double _x = chunk.cx * 24, _y = chunk.cy * 24, _z = chunk.cz * 24;
			glLoadMatrixd(mat4d::translation(_x, _y, _z) * cam.view);
			glBegin(GL_QUADS);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 0, 24);
			glVertex3f(24, 0, 24);
			glVertex3f(24, 0, 0);
			glEnd();
			gi.enableTexture2d();
			gi.enableLighting();
		} else {
			glColor3f(0.5, 0.5, 0.5);
			gi.disableTexture2d();
			gi.disableLighting();
			double _x = chunk.cx * 24, _y = chunk.cy * 24, _z = chunk.cz * 24;
			glLoadMatrixd(mat4d::translation(_x, _y, _z) * cam.view);
			glBegin(GL_QUADS);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 0, 24);
			glVertex3f(24, 0, 24);
			glVertex3f(24, 0, 0);
			glEnd();
			gi.enableTexture2d();
			gi.enableLighting();
		}
	}
	gi.disableLighting();
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
