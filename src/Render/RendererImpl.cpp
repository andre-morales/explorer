#include "RendererImpl.h"
#include "Renderer.h"
#include "Explorer/Explorer.h"
#include "Explorer/Game.h"
#include "Explorer/Window.h"
#include "Explorer/Camera.h"
#include "Explorer/ClientChunk.h"
#include "Explorer/ExplorerGUI.h"
#include "Game/Universe.h"
#include "Game/Planet.h"
#include "Render/Geometry.h"
#include "Render/GLContext.h"
#include "Render/Texture.h"
#include "GL/glew.h"

void RendererImpl::makeSun(Planet& pl){
	float time = pl.dayTime;
	float sunCycle = time * 360 * TO_RADS;
	float sunRotation = pl.sunXRotation * TO_RADS;
	float c_sin = std::sinf(sunCycle);
	float c_cos = std::cosf(sunCycle);
	float r_sin = std::sinf(sunRotation);
	float r_cos = std::cosf(sunRotation);
	sunVector = {c_sin * r_sin, -c_cos, c_sin * r_cos, 0};

	vec4f sunColor {1.0, 0.98, 0.83, 1.0f};
	float dtime = std::abs(time - 0.5f) * 4.0f;
	//std::cout << (dtime) << "\n";
	float sunFactor;
	if(dtime < 1) {
		sunFactor = 1;
	} else {
		sunFactor = std::max(0.0f, 1 - (dtime - 1) * 5 );
	}

	vec4f finalColor = sunColor * sunFactor;
	sunDiffuse = finalColor * 0.4f;
	sunAmbient = finalColor * 0.6f;
}

void RendererImpl::drawGame11(GLContext& gi){
	using namespace Geometry;
	Explorer& ex = This.explorer;
	Game& gin = *ex.gameInstance;

	if(!gin.universe) return;
	Universe& uni = *gin.universe;
	if(uni.planets.size() == 0) return;
	Planet& pl = *(uni.planets[0]);


	// Setup projection
	Camera& cam = *gin.camera;
	cam.aspect = This.window->getAspectRatio();

	cam.makeProjection();
	cam.makeRotatedView();
	cam.makeView();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(cam.projection.mat);

	// Setup just camera roation
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(cam.rotatedView);

	// Skybox
	gi.enableTexture2d();
	gi.enableVertsArray();
	gi.enableUVsArray();
	gi.disableDepthTesting();
	gi.disableFaceCulling();

	bindTexture("skybox");
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

	glVertexPointer(3, GL_FLOAT, 0, Geometry::Quad::verts3_tris);
	glTexCoordPointer(2, GL_FLOAT, 0, Geometry::Quad::uvs);
	glDrawArrays(GL_QUADS, 0, 4);
	glPopMatrix();

	// Moon
	bindTexture("moon");
	glRotatef(-pl.sunXRotation, 0, 1, 0);
	glRotatef(pl.dayTime * 360 + 90, 1, 0, 0);
	glScalef(0.42, 0.42, 1);
	glTranslatef(0, 0, -1);
	glVertexPointer(3, GL_FLOAT, 0, Geometry::Quad::verts3_tris);
	glTexCoordPointer(2, GL_FLOAT, 0, Geometry::Quad::uvs);
	glDrawArrays(GL_QUADS, 0, 4);
	gi.disableBlending();

	// Render chunks
	glLoadMatrixf(cam.view.mat);

	glLightfv(GL_LIGHT0, GL_POSITION, sunVector.vec);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiffuse.vec);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sunAmbient.vec);

	//const float moonpos[] = {sindf(sunAngle + 180) * sindf(pl.sunXRotation), sindf(sunAngle + 90), sindf(sunAngle  + 180) * cosdf(pl.sunXRotation), 0};
	//glLightfv(GL_LIGHT1, GL_POSITION, moonpos);

	gi.enableFaceCulling();
	gi.enableDepthTesting();
	gi.disableAlphaTesting();

	gi.setColorsArray(This.chunkColors);
	gi.setLighting(This.chunkLighting);
	gi.setNormalsArray(This.chunkLighting);

	bindTexture("blocks_atlas");
	for(Chunk* chunk_ : pl.chunkSet){
		Chunk& chunk = *chunk_;

		if(chunk.batchedCorners) {
			glPushMatrix();
			glTranslatef(chunk.cx * 24, chunk.cy * 24, chunk.cz * 24);
			glVertexPointer(3, GL_FLOAT, 0, chunk.batchedVerts);
			glTexCoordPointer(2, GL_FLOAT, 0, chunk.batchedUVs);

			if(This.chunkColors) glColorPointer(3, GL_UNSIGNED_BYTE, 0, chunk.batchedColors);
			if(This.chunkLighting) glNormalPointer(GL_BYTE, 0, chunk.batchedNormals);
			glDrawArrays(GL_QUADS, 0, chunk.batchedVertsLen);
			glPopMatrix();
		}
	}

	gi.disableNormalsArray();
	gi.disableLighting();
	gi.disableColorsArray();
}

void RendererImpl::drawGame15(GLContext& gi){
	using namespace Geometry;

	Explorer& ex = This.explorer;
	Game& gin = *ex.gameInstance;

	if(!gin.universe) return;
	Universe& uni = *gin.universe;
	if(uni.planets.size() == 0) return;
	Planet& pl = *(uni.planets[0]);

	// Setup projection
	Camera& cam = *gin.camera;
	cam.aspect = This.window->getAspectRatio();

	cam.makeProjection();
	cam.makeRotatedView();
	cam.makeView();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(cam.projection.mat);

	// Setup just camera roation
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(cam.rotatedView);

	// Skybox
	gi.enableTexture2d();
	gi.enableVertsArray();
	gi.enableUVsArray();
	gi.disableDepthTesting();
	gi.disableFaceCulling();

	bindTexture("skybox");
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

	uint32 quadbuffer = gi.buffers["geom_quad"];
	glBindBuffer(GL_ARRAY_BUFFER, quadbuffer);
	glVertexPointer(3, GL_FLOAT, 20, 0);
	glTexCoordPointer(2, GL_FLOAT, 20, (voidptr)(3 * 4));
	glDrawArrays(GL_QUADS, 0, 4);
	glPopMatrix();

	// Moon
	bindTexture("moon");
	glRotatef(-pl.sunXRotation, 0, 1, 0);
	glRotatef(pl.dayTime * 360 + 90, 1, 0, 0);
	glScalef(0.42, 0.42, 1);
	glTranslatef(0, 0, -1);
	glVertexPointer(3, GL_FLOAT, 20, 0);
	glTexCoordPointer(2, GL_FLOAT, 20, (voidptr)(3 * 4));
	glDrawArrays(GL_QUADS, 0, 4);
	gi.disableBlending();

	// Render chunks
	glLoadMatrixf(cam.view.mat);
	float sunAngle = pl.dayTime * 360;

	const float sunpos[] = {sindf(sunAngle) * sindf(pl.sunXRotation), sindf(sunAngle - 90), sindf(sunAngle) * cosdf(pl.sunXRotation), 0};
	glLightfv(GL_LIGHT0, GL_POSITION, sunpos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiffuse.vec);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sunAmbient.vec);

	const float moonpos[] = {sindf(sunAngle + 180) * sindf(pl.sunXRotation), sindf(sunAngle + 90), sindf(sunAngle  + 180) * cosdf(pl.sunXRotation), 0};
	glLightfv(GL_LIGHT1, GL_POSITION, moonpos);

	gi.disableFaceCulling();
	gi.enableDepthTesting();
	gi.disableAlphaTesting();

	gi.setColorsArray(This.chunkColors);
	gi.setLighting(This.chunkLighting);
	gi.setNormalsArray(This.chunkLighting);

	bindTexture("blocks_atlas");

	auto glv = This.window->glSetting;
	for(Chunk* chunk_ : pl.chunkSet){
		Chunk& chunk = *chunk_;

		if(This.debugChunkState || chunk.batchedCorners){
			glPushMatrix();
			glTranslatef(chunk.cx * 24, chunk.cy * 24, chunk.cz * 24);

			if(chunk.batchedCorners){		
				if(!chunk.vbo) uploadChunkVBO(&chunk);
				glBindBuffer(GL_ARRAY_BUFFER, chunk.vbo);

				glVertexPointer (3, GL_FLOAT, 28, (voidptr)0);
				glTexCoordPointer(2, GL_FLOAT, 28, (voidptr)12);		
				if(This.chunkColors) glColorPointer(3, GL_UNSIGNED_BYTE, 28, (voidptr)20);
				if(This.chunkLighting) glNormalPointer(GL_BYTE, 28, (voidptr)24);	

				glDrawArrays(GL_QUADS, 0, chunk.batchedVertsLen);
			} else if(This.debugChunkState){
				
				bool l_ = gi.lighting;

				gi.disableLighting();
				gi.disableNormalsArray();
				gi.disableTexture2d();

				if(chunk.batchedCenter){
					glBindBuffer(GL_ARRAY_BUFFER, 0);

					glVertexPointer (3, GL_FLOAT, 0, chunk.batchedVerts);
					glColorPointer(3, GL_UNSIGNED_BYTE, 0, chunk.batchedColors);
					glDrawArrays(GL_QUADS, 0, chunk.batchedVertsLen);

				} else {
					bool c_ = gi.colorsArray;
					gi.disableColorsArray();

					if(chunk.blocks){
						glColor3f(0.8, 0.1, 1.0);
					} else {
						glColor3f(0.1, 0.1, 1.0);
					}
					glBegin(GL_QUADS);
					glVertex3f(0, 0, 0);
					glVertex3f(0, 0, 24);
					glVertex3f(24, 0, 24);
					glVertex3f(24, 0, 0);
					glEnd();

					gi.setColorsArray(c_);
				}
				gi.setLighting(l_);
				gi.setNormalsArray(l_);
				gi.enableTexture2d();
			} 
			glPopMatrix();
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	gi.disableNormalsArray();
	gi.disableLighting();
	gi.disableColorsArray();
}

void RendererImpl::drawGame20(GLContext& gi){
	using namespace Geometry;

	Explorer& ex = This.explorer;
	Game& gin = *ex.gameInstance;

	if(!gin.universe) return;
	Universe& uni = *gin.universe;
	if(uni.planets.size() == 0) return;
	Planet& pl = *(uni.planets[0]);

	// Setup projection
	Camera& cam = *gin.camera;
	cam.aspect = This.window->getAspectRatio();

	cam.makeProjection();
	cam.makeRotatedView();
	cam.makeView();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(cam.projection.mat);

	// Setup just camera roation
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(cam.rotatedView);

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
	glLoadIdentity();
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

	gi.setColorsArray(This.chunkColors);
	gi.setLighting(This.chunkLighting);
	gi.setNormalsArray(This.chunkLighting);

	bindTexture("blocks_atlas");
	auto glv = This.window->glSetting;
	if(glv == GL::C2_0) {
		gi.bindShader("world");
		for(Chunk* chunk_ : pl.chunkSet){
			Chunk& chunk = *chunk_;

			if(chunk.batchedCorners){
				double _x = chunk.cx * 24, _y = chunk.cy * 24, _z = chunk.cz * 24;
				glLoadMatrixf(mat4d::translation(_x, _y, _z) * cam.view);
				if(!chunk.vbo) uploadChunkVBO(&chunk);
				glBindBuffer(GL_ARRAY_BUFFER, chunk.vbo);

				glVertexPointer (3, GL_FLOAT, 28, (voidptr)0);
				glTexCoordPointer(2, GL_FLOAT, 28, (voidptr)12);		
				if(This.chunkColors) glColorPointer(3, GL_UNSIGNED_BYTE, 28, (voidptr)20);
				if(This.chunkLighting) glNormalPointer(GL_BYTE, 28, (voidptr)24);	

				glDrawArrays(GL_QUADS, 0, chunk.batchedVertsLen);
			}
		}
		gi.bindShader(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	gi.disableNormalsArray();
	gi.disableLighting();
	gi.disableColorsArray();
}

void RendererImpl::uploadChunkVBO(Chunk* ch){
	glGenBuffers(1, &ch->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, ch->vbo);

	// Length of buffers in bytes.
	uintptr vertsLength = ch->batchedVertsLen * (4 * 3);
	uintptr uvsLength = ch->batchedVertsLen * (4 * 2);
	uintptr colorsLength = ch->batchedVertsLen * (4);
	uintptr normalsLength = ch->batchedVertsLen * (4);

	uintptr vboSize = vertsLength + uvsLength + colorsLength + normalsLength;

	byte* newData = new byte[vboSize];
	uintptr vertsPtr = (uintptr)ch->batchedVerts;
	uintptr uvsPtr = (uintptr)ch->batchedUVs;
	uintptr colorsPtr = (uintptr)ch->batchedColors;
	uintptr normalsPtr = (uintptr)ch->batchedNormals;
	for(int i = 0; i < ch->batchedVertsLen; i++) {

		memcpy(newData + i * 28,      (voidptr)(vertsPtr   + i * 12), 12);
		memcpy(newData + i * 28 + 12, (voidptr)(uvsPtr     + i *  8),  8);
		memcpy(newData + i * 28 + 20, (voidptr)(colorsPtr  + i *  3),  3);
		memcpy(newData + i * 28 + 24, (voidptr)(normalsPtr + i *  3),  3);	

	}
	glBufferData(GL_ARRAY_BUFFER, vboSize, newData, GL_STATIC_DRAW);
	delete[] newData;
}

void RendererImpl::renderUI(GLContext& gi){
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

	This.explorer.ui->render();
}

void RendererImpl::bindTexture(const char* tn){
	auto tex = This.textures.find(tn);
	if(tex != This.textures.end()) tex->second->bind();
}