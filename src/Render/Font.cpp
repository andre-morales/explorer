#include "Font.h"
#include "Renderer.h"
#include "TextureAtlas.h"
#include "Explorer/Window.h"
#include "Render/Geometry.h"
#include "Render/GLContext.h"
#include "Render/Color.h"
#include "GL/glew.h"

Font::Font(){
	aspect = 0;
	blending = true;
}
Font::Font(Shared<TextureAtlas> atlas) : Font(){
	setTexture(atlas);
}

void Font::setTexture(Shared<TextureAtlas> tex){
	this->fontAtlas = tex;
	fontAtlas->getTileSize(&charWidth, &charHeight);
	fontAtlas->getTileScale(&charSW, &charSH);
	aspect = fontAtlas->tileWidth / (float)fontAtlas->tileHeight;
}

void Font::drawString(GLContext& glc, const std::string& text, const Color& cl){
	glColor4f(cl.r, cl.g, cl.b, cl.a);
	glTranslatef(0.5, 0.5, 0);
	if(blending) glc.enableBlending();
	if(glc.version >= GL::C1_5) {
		glc.bindArrayBuffer("geom_quad");
		glVertexPointer(2, GL_FLOAT, 20, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	} else {
		glVertexPointer(2, GL_FLOAT, 0, Geometry::Quad::verts2_tris);
	}
	
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
					glColor4f(cl.r*0.50, cl.g*1.000, cl.b*0.50, cl.a);
					continue;
				case 'c':
					glColor4f(cl.r*1.00, cl.g*0.300, cl.b*0.30, cl.a);
					continue;
				case 'e':
					glColor4f(cl.r*1.00, cl.g*1.000, cl.b*0.30, cl.a);
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
			u+charSW, v,
			u+charSW, v+charSH,
			u, v+charSH,
		};
		glTexCoordPointer(2, GL_FLOAT, 0, uvs);
		glDrawArrays(GL_QUADS, 0, 4);
		glTranslatef(1, 0, 0);
		chars++;
	}
	glTranslatef(-chars - 0.5, -lines - 0.5, 0);
	glc.disableBlending();
}