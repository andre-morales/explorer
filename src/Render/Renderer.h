#pragma once
#include "ilib/mem.h"
#include "ilib/types.h"
#include "Render/GL.h"
#include "ilib/obs_map.h"

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
	obs_map<std::string, Shared<Texture>> textures;
	Shared<Font> font;

	bool chunkColors = true;

	Renderer(Explorer&);
	~Renderer();

	void init(GL);
	void loadResources();
	void begin();
	void shutdown();
	void render();
	void bindTexture(const char*);
	Shared<Texture> getTexture(const char*);
	Shared<TextureAtlas> getAtlas(const char*);
	Shared<Texture> loadTexture(const char*);
	Shared<Texture> loadTexture(const char*, bool, bool);
	Shared<TextureAtlas> loadAtlas(const char*, uint32, uint32);
private:
    void drawGame11();
    void log(const std::string&, const std::string&);
};
