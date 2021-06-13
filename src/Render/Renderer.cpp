#include "Renderer.hpp"
#include "Explorer/Explorer.h"
#include "Explorer/Game.h"
#include "Explorer/Window.h"
#include "Explorer/Camera.h"
#include "Game/Universe.h"
#include "Game/Planet.h"
#include "Game/Chunk.h"
#include "GUI/GUI.h"
#include "Render/GLContext.h"
#include "Render/TextureAtlas.h"
#include "Render/Image.h"
#include "Render/Font.h"
#include "Exception.h"
#include "GL/glew.h"
#include <string>
#include <iostream>

Renderer::Renderer(Explorer& exp) : explorer(exp){
	window = mkShared<Window>(800, 600, "Explorer");
}

Renderer::~Renderer(){

}

void Renderer::init(GL gl){
	window->setGL(gl);
	window->create();
	window->acquireContext();
	context = mkShared<GLContext>(window);

	fontAtlas = mkShared<TextureAtlas>(7, 9);
	fontAtlas->set(Image("res\\fonts.png"));
	font = mkShared<Font>(fontAtlas);

	blockAtlas = loadAtlas("blocks_atlas", 32, 32);

	loadTexture("title_bg");
	loadTexture("gui_button");
	loadTexture("gui_panel");
	loadTexture("gui_panel2");
    loadTexture("skybox");

	if(glewInit() != GLEW_OK){
		throw Exception("GLEW initialization failed!");
	}
}

void Renderer::begin(){
	window->setVisible(true);
	glClearColor(1.0, 0.0, 0.0, 1.0);
}

void Renderer::shutdown(){
	textures.clear();
	window->destroy();
}

void Renderer::render(){
	glViewport(0, 0, window->getWidth(), window->getHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(explorer.gameInstance){
        drawGame11();
	}

    for (auto& gui : explorer.attachedGUIs) {
        gui->render();
    }
}

void Renderer::drawGame11(){
	Explorer& ex = explorer;
	GLContext& gi = *context;
    Game& gin = *ex.gameInstance;
    Camera& cam = *gin.camera;
	Player& mp = *gin.player;

	float aspect = window->getWidth() / (float)window->getHeight();
	cam.makeView();
	cam.makeTransform();

	glColor3f(1, 1, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(cam.makeProjection(aspect, 90));

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
	gi.disableDepthTesting();

    textures["skybox"]->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glVertexPointer(3, GL_FLOAT, 0, skybox_verts);
    glTexCoordPointer(2, GL_FLOAT, 0, skybox_uvs);
    glDrawArrays(GL_QUADS, 0, 24);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(cam.makeProjection(aspect, 90));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixd(cam.view);

	gi.disableTexture2d();
    glBegin(GL_QUADS);
    float z = -1;
    glVertex3f(0, 0, z);
    glVertex3f(1, 0, z);
    glVertex3f(1, 1, z);
    glVertex3f(0, 1, z);
    glEnd();
	/*float v[] = {
	    0, 0, 0,
	    1, 0, 0,
	    1, 1, 0,
	    0, 1, 0};
	glVertexPointer(3, GL_FLOAT, 0, v);
	glDrawArrays(GL_QUADS, 0, 4);*/

	/*if(lighting) makeSunLight();

	// Draw Sun
	if(drawSky){
		glBindTexture(GL_TEXTURE_2D, sunTexture->glId);
		gi.setBlending(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		gi.enableBlending();
		glRotatef(135, 0, 1, 0);
		glRotatef(wr.sunAngle+90, 1, 0, 0);

		glTranslatef(0, 0, -1);
		glScalef(0.3, 0.3, 0.3);
		glVertexPointer(2, GL_FLOAT, 0, Quad::verts2_tris);
		glTexCoordPointer(2, GL_FLOAT, 0, Quad::uvs);
		glDrawArrays(GL_QUADS, 0, 4);
	}
	gi.disableBlending();*/

	// Draws Slices Geometry
	/*gi.setLighting(lighting);
	gi.setNormalsArray(lighting);*/

	/*if(ambientOcclusion){
		gi.setShadingModel(GL_SMOOTH);
		gi.enableColorsArray();
		gi.enableColorMaterial();
	} else {
		gi.setShadingModel(GL_FLAT);
	}
	gi.enableAlphaTesting();
	if(fog) gi.enableFog();*/
    gi.enableTexture2d();
	gi.setFaceCulling(false);
	gi.enableDepthTesting();
    gi.disableAlphaTesting();
	blockAtlas->bind();

	// Render Slices
	for(Chunk* chunk_ : pl.chunkSet){
		Chunk& chunk = *chunk_;
		/*if(slice.state == Slice::State::FINISHED_QUICK_BATCH){
			slice.batch(GL::V1_1);
		}
		if(occludeFrustumSlice(cam.transform, slice)) continue;

		if(slice.state == Slice::State::FINISHED_UNBATCHED){
			slice.state = Slice::State::FINISHED_TO_BATCH;
		}
		if(slice.state == Slice::State::FINISHED_BATCHED){
			if(slice.vertsLock.try_lock()){
				fRenderedSlices++;*/

				double _x = chunk.cx * 24, _y = chunk.cy * 24, _z = chunk.cz * 24;
				glLoadMatrixd(mat4d::translation(_x, _y, _z) * cam.view);
				glVertexPointer(3, GL_FLOAT, 0, chunk.batchedVerts);
				glTexCoordPointer(2, GL_FLOAT, 0, chunk.batchedUVs);

				//if(lighting) glNormalPointer(GL_BYTE, 0, slice.normals);
				//if(ambientOcclusion) glColorPointer(3, GL_UNSIGNED_BYTE, 0, slice.colors);
				glDrawArrays(GL_QUADS, 0, chunk.batchedVertsLen);
				//slice.vertsLock.unlock();
			/*}
		}*/
	}


	/*gi.disableFog();
	gi.disableColorMaterial();
	gi.disableColorsArray();
	gi.disableNormalsArray();
	gi.disableAlphaTesting();
	gi.disableLighting();
	gi.disableFaceCulling();
	gi.disableTexture2d();

	// Draw Selection
	if(mp.isBlockSelected){
		glLineWidth(3);
		glColor3f(0.9, 0.9, 0.9);
		cam.makeView();
		glLoadMatrixd(mat4d::scaling(1.003, 1.003, 1.003) * (mat4d::translation(mp.selectedBlock.x, mp.selectedBlock.y, mp.selectedBlock.z) * cam.view));
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glVertexPointer(3, GL_FLOAT, 0, Cube::verts_tris);
		glDrawArrays(GL_QUADS, 0, 24);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor3f(1, 1, 1);
	}

	// Draw UI Elements
	gi.disableFaceCulling();
	gi.disableDepthTesting();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Draw Vignette
	gi.enableTexture2d();
	gi.enableBlending();
	gi.setBlending(GL_ZERO, GL_SRC_COLOR);
	glBindTexture(GL_TEXTURE_2D, vignetteTexture->glId);
	glVertexPointer(2, GL_FLOAT, 0, vignette_verts);
	glTexCoordPointer(2, GL_SHORT, 0, Quad::uvs_s);
	glDrawArrays(GL_QUADS, 0, 4);

	// Draw Crosshair
	gi.disableTexture2d();
	glScalef(1, -gi.aspect, 1);
	gi.setBlending(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glVertexPointer(2, GL_FLOAT, 0, crosshair_verts);
	glDrawArrays(GL_QUADS, 0, 8);

	// Draw Debug Screen
	gi.disableBlending();
	if(mp.infoScreen){
		gi.enableAlphaTesting();
		gi.enableTexture2d();
		glTranslatef(-1, -1.0/gi.aspect, 0);
		glScalef(font->aspect*0.03, 0.03, 1);
		font->drawShadowString(getDebugText(), {1, 1, 1, 1}, {0.75, 0.75, 0.75, 1}, 0.1, 0.1);
	}*/
}

Shared<Texture> Renderer::loadTexture(const char* str){
	auto path = std::string("res\\") + str + std::string(".png");
	auto name = std::string(str);
	auto texture = mkShared<Texture>(Image(path));
	textures.insert(std::pair<std::string, Shared<Texture>>(name, texture));
	return texture;
}

Shared<TextureAtlas> Renderer::loadAtlas(const char* str, uint32 tw, uint32 th){
	auto path = std::string("res\\") + str + std::string(".png");
	auto name = std::string(str);
	auto texture = mkShared<TextureAtlas>(tw, th);
	texture->set(Image(path));
	return texture;
}
