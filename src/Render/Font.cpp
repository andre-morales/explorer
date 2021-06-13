#include "Font.h"
#include "Renderer.h"
#include "TextureAtlas.h"
#include "Explorer/Window.h"
#include "Render/Geometry.h"
#include "Render/GLContext.h"
#include "Render/Color.h"
#include <GL/glew.h>
#include <cstring>

Font::Font(Shared<TextureAtlas> atlas){
	this->fontAtlas = atlas;
	fontAtlas->getTileSize(&chWidth, &chHeight);
	aspect = atlas->tileWidth / (float)atlas->tileHeight;
}

void Font::drawString(GLContext& glc, const std::string& text, const Color& cl){
	glAlphaFunc(GL_GREATER, 0.5);
	glc.enableAlphaTesting();
	glColor4f(cl.r, cl.g, cl.b, cl.a);
	glVertexPointer(2, GL_FLOAT, 0, Quad::uvs);
	fontAtlas->bind();

	float u = 0, v = 0;
	uint8 chars = 0; // Characters typed horizontally.
	uint8 lines = 0;
	bool formatting = false;
	for(const char& c : text){
		if(formatting){
			formatting = false;
			switch(c){
				case '2':
					glColor4f(cl.r*0.25, cl.g*0.875, cl.b*0.25, cl.a);
					continue;
				case 'a':
					glColor4f(cl.r*0.5, cl.g, cl.b*0.5, cl.a);
					continue;
				default:
				case 'r':
					glColor4f(cl.r, cl.g, cl.b, cl.a);
					continue;
			}
		}
		if(c == '\n'){
			glTranslatef(-chars, 1, 0);
			chars = 0;
			lines++;
			continue;
		}
		if(c == '$'){
			formatting = true;
			continue;
		}
		fontAtlas->getTileUV(c - 32, &u, &v);
		const float uvs[] = {
			u, v,
			u+chWidth, v,
			u+chWidth, v+chHeight,
			u, v+chHeight,
		};
		glTexCoordPointer(2, GL_FLOAT, 0, uvs);
		glDrawArrays(GL_QUADS, 0, 4);
		glTranslatef(1, 0, 0);
		chars++;
	}
	glTranslatef(-chars, -lines, 0);
}

void Font::drawShadowString(GLContext& glc, const std::string& text, const Color& col, const Color& sha, float sx, float sy){
	Color cl = col;
	Color sh = sha;

	glTranslatef(-1, 0, 0);
	glVertexPointer(2, GL_FLOAT, 0, Quad::uvs);
	glBindTexture(GL_TEXTURE_2D, fontAtlas->glId);

	float u = 0, v = 0;
	uint8 chars = 0; // Characters typed horizontally.
	uint8 lines = 0;
	bool formatting = false;
	for(const char& c : text){
		if(formatting){
			formatting = false;
			switch(c){
				case '2':
					cl = {col.r*0.25f, col.g*0.875f, col.b*0.25f, col.a};
					sh = {sha.r*0.25f, sha.g*0.875f, sha.b*0.25f, sha.a};
					continue;
				case 'a':
					cl = {col.r*0.5f, col.g, col.b*0.5f, col.a};
					sh = {sha.r*0.5f, sha.g, sha.b*0.5f, sha.a};
					continue;
				default:
				case 'r':
					cl = col;
					sh = sha;
					continue;
			}
		}
		if(c == '\n'){
			glTranslatef(-chars, 1, 0);
			chars = 0;
			lines++;
			continue;
		}
		if(c == '$'){
			formatting = true;
			continue;
		}
		fontAtlas->getTileUV(c - 32, &u, &v);
		const float uvs[] = {
			u, v,
			u+chWidth, v,
			u+chWidth, v+chHeight,
			u, v+chHeight,
		};
		glTexCoordPointer(2, GL_FLOAT, 0, uvs);
		// Shadow
		glColor4f(sh.r, sh.g, sh.b, sh.a);
		glTranslatef(sx + 1, sy, 0);
		glDrawArrays(GL_QUADS, 0, 4);
		// Char
		glColor4f(cl.r, cl.g, cl.b, cl.a);
		glTranslatef(-sx, -sy, 0);
		glDrawArrays(GL_QUADS, 0, 4);
		chars++;
	}
	glTranslatef(-chars, -lines, 0);
}
