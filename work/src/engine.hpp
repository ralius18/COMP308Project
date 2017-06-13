#pragma once

#include "light.hpp"
#include "geometryMain.hpp"


class Engine
{
public:
	Engine(GeometryMain, GLFWwindow*);

	void mainLoop();
	void render();
	void run();
	void update();

	static Engine* engine;
	GeometryMain* geometryMain;
	GLFWwindow* window;

	Light* light;

	//offscreen rendering
	bool shaderSupported;
	GLhandleARB shader;

	float uniformLightX;
	float uniformLightY;
	float uniformExposure;
	float uniformDecay;
	float uniformDensity;
	float uniformWeight;

private:
	short gameOn;
	
	void getLightScreenCoord();

	unsigned int localLight;
	unsigned int localExposure;
	unsigned int localDecay;
	unsigned int localDensity;
	unsigned int localWeight;
	unsigned int localTexture;

	void createScreenCopyTexture();
	GLuint screenCopyTextureId;
	void copyFrameBufferToTexture();

};