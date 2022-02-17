#pragma once
#include "ilib/mem.h"
#include "ilib/types.h"
#include "Render/GL.h"
#include "ilib/obs_unordered_map.h"

class Window;
class GLContext;
class Texture;
class TextureAtlas;
class Font;

class Renderer {
private:
	class RendererImpl* impl_;
	RendererImpl& impl;
public:
	class Explorer& explorer;
	Shared<Window> window;
	Shared<GLContext> context;
	obs_unordered_map<std::string, Shared<Texture>> textures;
	Shared<Font> font;

	bool chunkColors = true, chunkLighting = true;
	bool debugChunkState = true;

	Renderer(Explorer&);
	~Renderer();

	void init(GL::GLContext);
	void loadResources();
	void begin();
	void shutdown();
	void render();

	void freeChunk(class Chunk*);

	sh<Texture> getTexture(const char*);
	sh<TextureAtlas> getAtlas(const char*);

	sh<Texture> loadTexture(const std::string& name);
	sh<Texture> loadTexture(const std::string& name, const std::string& file);

	sh<Texture> loadTexture(const std::string& name, bool, bool);
	sh<Texture> loadTexture(const std::string& name, const std::string& file, bool min, bool mag);
	sh<TextureAtlas> loadAtlas(const char*, uint32, uint32);

	sh<class Shader> loadShader(const std::string& name);
	sh<Shader> loadShader(const std::string& name, const std::string& file);
private:
	GL glVersion;
	void log(const std::string&, const std::string&);
};
