#pragma once

#include "light.hpp"

class Engine
{
public:
	Engine();

	void mainLoop();
	void render();
	void update();

	static Engine* engine;

	Light* light;

	float uniformLightX;
	float uniformLightY;
	float uniformExposure;
	float uniformDecay;
	float uniformDensity;
	float uniformWeight;
	GLuint fboInfo;
};