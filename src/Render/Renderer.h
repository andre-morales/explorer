#pragma once
#include "ilib/mem.h"
#include "ilib/types.h"
#include "Render/GL.h"
#include <map>

class Explorer;
class Window;
class GLContext;
class Texture;
class TextureAtlas;
class Font;

class Renderer {
public:
    Explorer& explorer;
	Shared<Window> window;
	Shared<GLContext> context;
	Shared<TextureAtlas> fontAtlas;
	Shared<TextureAtlas> blockAtlas;
	std::map<std::string, Shared<Texture>> textures;
	Shared<Font> font;

	Renderer(Explorer&);
	~Renderer();

	void init(GL);
	void begin();
	void shutdown();
	void render();
	Shared<Texture> loadTexture(const char*);
	Shared<TextureAtlas> loadAtlas(const char*, uint32, uint32);
private:
	std::string getDebugText();
    void drawGame11();
};
