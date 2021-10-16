#pragma once
#include "Render/GL.h"
#include "ilib/mem.h"
#include "ilib/types.h"
#include "ilib/obs_unordered_map.h"

class Window;

class GLContext {
public:
	Weak<Window> window;
	obs_unordered_map<std::string, uint32> buffers;
	obs_unordered_map<std::string, sh<class Shader>> shaders;
	GL::GLContext version;

	GLContext(GL::GLContext, Shared<Window>);
	~GLContext();

	int getWidth();
	int getHeight();
	float getAspect();

	// -- OpenGL Functions --
	void enableFog();
	void disableFog();

	void enableTexture2d();
	void disableTexture2d();

	void enableAlphaTesting();
	void disableAlphaTesting();

	void enableDepthTesting();
	void disableDepthTesting();

	void enableBlending();
	void disableBlending();
	void setBlending(uint32, uint32);

	void setShadingModel(uint32);

	bool lighting = false;
	void setLighting(bool);
	void enableLighting();
	void disableLighting();

	void setFaceCulling(bool);
	void enableFaceCulling();
	void disableFaceCulling();

	bool colorsArray = false;
	void enableVertsArray();
	void disableVertsArray();

	void enableUVsArray();
	void disableUVsArray();

	void setNormalsArray(bool);
	void enableNormalsArray();
	void disableNormalsArray();

	void setColorsArray(bool);
	void enableColorsArray();
	void disableColorsArray();

	void enableColorMaterial();
	void disableColorMaterial();

	// OpenGL 1.5
	void bindArrayBuffer(const std::string&);

	// OpenGL 2.0
	uint32 shaderId;
	void bindShader(uint32);
	void bindShader(const std::string&);

	// OpenGL 3.0
	uint32 frameBufferId;
	void bindFramebuffer(uint32);	
private:
	bool fog, texture2d, depthTesting, alphaTesting, blending, faceCulling;
	bool vertsArray, uvsArray, normalsArray;
	uint32 blendingSource, blendingDestination, shadingModel;
	uint32 texture2dId, arrayBufferId;
	bool colorMaterial;
};
