#pragma once

#include "light.hpp"

class Engine
{
public:
	Engine();

	void mainLoop();
	void render();
	void run();

	static Engine* engine;

	Light* light;

	bool shaderSupported;
	GLhandleARB shader;

	//Frame-buffer object
	bool fboSupported;
	bool fboUsed;

	float uniformLightX;
	float uniformLightY;
	float uniformExposure;
	float uniformDecay;
	float uniformDensity;
	float uniformWeight;
	GLuint fboId;

private:
	short gameOn;
	void update();
	
	void getLightScreenCoord();

	unsigned int glsl_loc_light;
	unsigned int glsl_loc_exposure;
	unsigned int glsl_loc_decay;
	unsigned int glsl_loc_density;
	unsigned int glsl_loc_weight;
	unsigned int glsl_loc_texture;

	void createScreenCopyTexture();
	GLuint screenCopyTextureId;
	void copyFrameBufferToTexture();

	//FBO
	void generateFBO();
	GLuint fboTextureId;
};