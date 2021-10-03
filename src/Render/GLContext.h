#pragma once
#include "ilib/mem.h"
#include "ilib/types.h"

class Window;

class GLContext {
public:
	Weak<Window> window;

	GLContext(Shared<Window>);
	~GLContext();

	int getWidth();
	int getHeight();
	float getAspect();

	// -- OpenGL Functions --
	bool fog, texture2d, depthTesting, alphaTesting, blending, lighting, faceCulling;
	bool vertsArray, uvsArray, normalsArray, colorsArray;
	uint32 blendingSource, blendingDestination, shadingModel;
	uint32 texture2dId, arrayBufferId;
	bool colorMaterial;

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

	void setLighting(bool);
	void enableLighting();
	void disableLighting();

	void setFaceCulling(bool);
	void enableFaceCulling();
	void disableFaceCulling();

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

	// OpenGL 2.0
	uint32 shaderId;
	void bindShader(uint32);

	// OpenGL 3.0
	uint32 frameBufferId;
	void bindFramebuffer(uint32);	
};
