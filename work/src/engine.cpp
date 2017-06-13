#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "engine.hpp"
#include "math.h"
#include "geometry.hpp"

using namespace std;

Engine* Engine::engine;

#define renderWidth 640
#define renderHeight 480
#define OFF_SCREEN_RENDER_RATIO 1

Engine::Engine(GeometryMain gm, GLFWwindow* window)
{
	glClearColor(0.2, 0.2, 0.28, 1);
	glEnable(GL_TEXTURE9);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5f);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	engine = this;

	shaderSupported = false;

	light = new Light();

	geometryMain = &gm;

	this->window = window;

	createScreenCopyTexture();

	if (shaderSupported) {
		//TODO: this stuff

		shader = glCreateProgramObjectARB();

		uniformExposure = 0.0034f;
		uniformDecay = 1.0f;
		uniformDensity = 0.84f;
		uniformWeight = 5.65f;

		localExposure = 0.0034f;
		localDecay = 1.0f;
		localDensity = 0.84f;
		localWeight = 5.65f;

		/*
		glUniform2fARB(localLight, uniformLightX, uniformLightY);
		glUniform1fARB(localExposure, uniformExposure);
		glUniform1fARB(localDecay, uniformDecay);
		glUniform1fARB(localDensity, uniformDensity);
		glUniform1fARB(localWeight, uniformWeight);
		glUniform1iARB(localTexture, screenCopyTextureId);*/

	}


	gameOn = 1;
}

void Engine::run()
{
	while (gameOn)
	{
		update();
		render();

		glfwSwapBuffers(window);
	}
}

void Engine::update() 
{

}

void Engine::render()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glColor4f(0, 1, 1, 1);

	glViewport(0, 0, renderWidth / OFF_SCREEN_RENDER_RATIO, renderHeight / OFF_SCREEN_RENDER_RATIO);

	glDisable(GL_TEXTURE9);
	glPushMatrix();
		glTranslatef(light->lightPos[0], light->lightPos[1], light->lightPos[2]);
		light->render();
	glPopMatrix();

	getLightScreenCoord();
	
	//Draw objects black with light
	glUseProgramObjectARB(0);

	//glEnable(GL_TEXTURE9);
	glDisable(GL_TEXTURE9);
	glColor4f(0, 0, 0, 1);
	glPushMatrix();
		geometryMain->renderGeometry();
	glPopMatrix();

	copyFrameBufferToTexture();
	/*
	//Draw scene with no light scattering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor4f(1, 1, 1, 1);
	glPushMatrix();
		geometryMain->renderGeometry();
	glPopMatrix();
	
	//Paint light scattering effect
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-renderWidth / 2, renderWidth / 2, -renderHeight / 2, renderHeight / 2, 0, 50000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_DEPTH_BUFFER_BIT);

	glActiveTextureARB(GL_TEXTURE9_ARB);
	glBindTexture(GL_TEXTURE9, screenCopyTextureId);


	glEnable(GL_TEXTURE9);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
		glVertex2f(-renderWidth / 2, -renderHeight / 2);

		glTexCoord2f(1, 0);
		glVertex2f(renderWidth / 2, -renderHeight / 2);

		glTexCoord2f(1, 1);
		glVertex2f(renderWidth / 2, renderHeight / 2);

		glTexCoord2f(0, 1);
		glVertex2f(-renderWidth / 2, renderHeight / 2);
	glEnd();
	*/
	glUseProgramObjectARB(0);
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

void Engine::createScreenCopyTexture()
{
	int width = renderWidth;
	int height = renderHeight;

	char* emptyData = (char*)malloc(width * height * 3 * sizeof(char)); //allocate memory for textures
	memset(emptyData, 0, width * height * 3 * sizeof(char)); //set memory

	glActiveTextureARB(GL_TEXTURE0);
	glGenTextures(1, &screenCopyTextureId);
	glBindTexture(GL_TEXTURE9, screenCopyTextureId);	//bind texture name
	glTexImage2D(GL_TEXTURE9, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, emptyData); //specify texture
	glTexParameteri(GL_TEXTURE9, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//use lerp for magnification and minification (below)
	glTexParameteri(GL_TEXTURE9, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	free(emptyData);	//release memory;
}

void Engine::copyFrameBufferToTexture() 
{
	glBindTexture(GL_TEXTURE9, screenCopyTextureId);
	glCopyTexSubImage2D(GL_TEXTURE9, 0, 0, 0, 0, 0, renderWidth, renderHeight);
}