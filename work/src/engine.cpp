#include "engine.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "math.h"
#include <algorithm>
using namespace std;

Engine* Engine::engine;

#define renderWidth 1024
#define renderHeight 768
#define OFF_SCREEN_RENDER_RATIO 2

Engine::Engine()
{
	fboId = 0;
	fboTextureId = 0;

	engine = this;

	shaderSupported = false;

	light = new Light();

	if (fboSupported)
		generateFBO();

	if (!fboUsed)
		createScreenCopyTexture();

	if (shaderSupported) {
		//TODO: this stuff
	}


	gameOn = 1;
}

void Engine::render()
{
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
		glTranslatef(light->lightPos[0], light->lightPos[1], light->lightPos[2]);
		light->render();
	glPopMatrix();

	getLightScreenCoord();

}

void Engine::getLightScreenCoord() 
{
	double modelview[16];
	double proj[16];
	double depthRange[2];
	GLint viewport[4];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, proj);
	glGetDoublev(GL_DEPTH_RANGE, depthRange);
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	GLdouble windowX = 0;
	GLdouble windowY = 0;
	GLdouble windowZ = 0;

	gluProject(light->lightPos[0], light->lightPos[1], light->lightPos[2], modelview, proj, viewport, &windowX, &windowY, &windowZ);

	uniformLightX = windowX / ((float)renderWidth / OFF_SCREEN_RENDER_RATIO);
	uniformLightY = windowY / ((float)renderWidth / OFF_SCREEN_RENDER_RATIO);
}