#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "engine.hpp"
#include "math.h"
#include "geometry.hpp"
#include "simple_shader.hpp"

using namespace std;
using namespace cgra;

Engine* Engine::engine;

#define renderWidth 640
#define renderHeight 480
#define OFF_SCREEN_RENDER_RATIO 2

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

	GLfloat fogColor[4] = { 0.2f ,0.2f ,0.28f ,1.0f };
	//Set fog 
	glFogi(GL_FOG_MODE, GL_LINEAR);		// Fog Mode
	glFogfv(GL_FOG_COLOR, fogColor);			// Set Fog Color
	glFogf(GL_FOG_DENSITY, 0.20f);				// How Dense Will The Fog Be
	glHint(GL_FOG_HINT, GL_FASTEST);			// Fog Hint Value
	glFogf(GL_FOG_START, 7000.0f);				// Fog Start Depth
	glFogf(GL_FOG_END, 10000.0f);				// Fog End Depth
	glEnable(GL_FOG);
	//End fog

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	engine = this;

	light = new Light();

	geometryMain = &gm;

	this->window = window;

	createScreenCopyTexture();

	m_nSamples = 3;		// Number of sample rays to use in integral equation
	m_Kr = 0.0025f;		// Rayleigh scattering constant
	m_Kr4PI = m_Kr*4.0f*PI;
	m_Km = 0.0010f;		// Mie scattering constant
	m_Km4PI = m_Km*4.0f*PI;
	m_ESun = 20.0f;		// Sun brightness constant
	m_g = -0.990f;		// The Mie phase asymmetry factor
	m_fExposure = 2.0f;

	m_fInnerRadius = 10.0f;
	m_fOuterRadius = 10.25f;
	m_fScale = 1 / (m_fOuterRadius - m_fInnerRadius);

	m_fWavelength[0] = 0.650f;		// 650 nm for red
	m_fWavelength[1] = 0.570f;		// 570 nm for green
	m_fWavelength[2] = 0.475f;		// 475 nm for blue
	m_fWavelength4[0] = powf(m_fWavelength[0], 4.0f);
	m_fWavelength4[1] = powf(m_fWavelength[1], 4.0f);
	m_fWavelength4[2] = powf(m_fWavelength[2], 4.0f);

	m_fRayleighScaleDepth = 0.25f;
	m_fMieScaleDepth = 0.1f;

	shaderSupported = true;
	if (shaderSupported) {
		//TODO: this stuff

		shader = makeShaderProgramFromFile({ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, { "../work/res/shaders/vert_simpleTexture.glsl", "../work/res/shaders/frag_simpleTexture.glsl" });

		glUseProgramObjectARB(shader);
			
		uniformExposure = 0.0034f;
		uniformDecay = 1.0f;
		uniformDensity = 0.84f;
		uniformWeight = 5.65f;

		localLight = glGetUniformLocationARB(shader, "lightPositionOnScreen");
		glUniform2fARB(localLight, uniformLightX, uniformLightY);
		localExposure = glGetUniformLocationARB(shader, "exposure");
		glUniform1fARB(localExposure, uniformExposure);
		localDecay = glGetUniformLocationARB(shader, "decay");
		glUniform1fARB(localDecay, uniformDecay);
		localDensity = glGetUniformLocationARB(shader, "density");
		glUniform1fARB(localDensity, uniformDensity);
		localWeight = glGetUniformLocationARB(shader, "weight");
		glUniform1fARB(localWeight, uniformWeight);
		localTexture = glGetUniformLocationARB(shader, "myTexture");
		glUniform1fARB(localTexture, screenCopyTextureId);
		/*
		cout << "Light: " << localLight << endl;
		cout << "Exposure: " << localExposure << endl;
		cout << "Decay: " << localDecay << endl;
		cout << "Density: " << localDensity << endl;
		cout << "Weight: " << localWeight << endl;
		cout << "Texture: " << localTexture << endl;
		*/
		GLuint atmosshader = makeShaderProgramFromFile({ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, { "../work/res/shaders/GroundFromAtmosphere.vert", "../work/res/shaders/GroundFromAtmosphere.frag" });
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

	glColor4f(1, 1, 1, 1);

	glViewport(0, 0, renderWidth / OFF_SCREEN_RENDER_RATIO, renderHeight / OFF_SCREEN_RENDER_RATIO);

	glDisable(GL_TEXTURE9);
	glPushMatrix();
		glEnable(GL_COLOR_MATERIAL);
			glTranslatef(light->lightPos[0], light->lightPos[1], light->lightPos[2]);
			light->render();
		glDisable(GL_COLOR_MATERIAL);
	glPopMatrix();

	getLightScreenCoord();
	
	//STEP 1 ------------------------
	//Draw objects black with light
	glUseProgramObjectARB(0);
	
	glEnable(GL_TEXTURE9);
	glColor4f(0, 0, 0, 1);
	glPushMatrix();
		geometryMain->renderGeometry();
	glPopMatrix();
	

	copyFrameBufferToTexture();
	
	glViewport(0, 0, renderWidth, renderHeight);
	glEnable(GL_TEXTURE9);
	
	//STEP 2 ------------------------
	//Draw scene with no light scattering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor4f(1, 1, 1, 1);
	glPushMatrix();
		geometryMain->toggleTextures(); //true
		geometryMain->renderGeometry();
		geometryMain->toggleTextures(); //false
	glPopMatrix(); 
	
	
	//STEP 3 ------------------------
	//Paint light scattering effect
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-renderWidth / 2, renderWidth / 2, -renderHeight / 2, renderHeight / 2, 0, 50000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_DEPTH_BUFFER_BIT);

	glActiveTextureARB(GL_TEXTURE8_ARB);
	glBindTexture(GL_TEXTURE9, screenCopyTextureId);

	//update values
	glUseProgramObjectARB(shader);
	glUniform2fARB(localLight, uniformLightX, uniformLightY); //mostly just this one
	glUniform1fARB(localExposure, uniformExposure);
	glUniform1fARB(localDecay, uniformDecay);
	glUniform1fARB(localDensity, uniformDensity);
	glUniform1fARB(localWeight, uniformWeight);
	glUniform1iARB(localTexture, 0);

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

	glUseProgramObjectARB(0);

	//glDisable(GL_BLEND);
	//glDisable(GL_TEXTURE9);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);
	
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

	GLint projectResult = gluProject(light->lightPos[0], light->lightPos[1], light->lightPos[2], modelview, proj, viewport, &windowX, &windowY, &windowZ);
	
	//if (projectResult == GL_TRUE)
		//cout << "Project Successful" << endl;
	//else
		//cout << "Project Failure" << endl;

	uniformLightX = windowX / ((float)renderWidth / OFF_SCREEN_RENDER_RATIO);
	uniformLightY = windowY / ((float)renderHeight / OFF_SCREEN_RENDER_RATIO);

	//cout << "Light Position: winX=" << windowX << " winY" << windowY << " x=" << uniformLightX << " y=" << uniformLightY << endl;
}

void Engine::createScreenCopyTexture()
{
	int width = renderWidth;
	int height = renderHeight;

	char* emptyData = (char*)malloc(width * height * 3 * sizeof(char)); //allocate memory for textures (*3 for rgb)
	memset(emptyData, 0, width * height * 3 * sizeof(char)); //set memory

	glActiveTextureARB(GL_TEXTURE8);
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