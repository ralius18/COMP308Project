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
//#include "GLUtil.h"

using namespace std;
using namespace cgra;
using namespace math;

Engine* Engine::engine;

#define renderWidth 640
#define renderHeight 480
#define OFF_SCREEN_RENDER_RATIO 1

//#define PI 3.14

Engine::Engine(GLFWwindow* window)
{
	glClearColor(0.2, 0.2, 0.28, 1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5f);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	//m_pBuffer.Init(1024, 1024, 0);
	//m_pBuffer.MakeCurrent();

	//GLfloat fogColor[4] = { 0.2f ,0.2f ,0.28f ,1.0f };
	//Set fog 
	//glFogi(GL_FOG_MODE, GL_LINEAR);		// Fog Mode
	//glFogfv(GL_FOG_COLOR, fogColor);			// Set Fog Color
	//glFogf(GL_FOG_DENSITY, 0.20f);				// How Dense Will The Fog Be
	//glHint(GL_FOG_HINT, GL_FASTEST);			// Fog Hint Value
	//glFogf(GL_FOG_START, 7000.0f);				// Fog Start Depth
	//glFogf(GL_FOG_END, 10000.0f);				// Fog End Depth
	//glEnable(GL_FOG);
	//End fog

	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	engine = this;

	light = new Light();

	geometryMain = new GeometryMain();

	this->window = window;

	createScreenCopyTexture();

	m_nSamples = 3;		// Number of sample rays to use in integral equation
	m_Kr = 0.0025f;		// Rayleigh scattering constant
	m_Kr4PI = m_Kr*4.0f*pi();
	m_Km = 0.0010f;		// Mie scattering constant
	m_Km4PI = m_Km*4.0f*pi();
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

	//m_pbOpticalDepth.MakeOpticalDepthBuffer(m_fInnerRadius, m_fOuterRadius, m_fRayleighScaleDepth, m_fMieScaleDepth);

	//m_shSkyFromSpace.Load("SkyFromSpace");
	//m_shSkyFromAtmosphere.Load("SkyFromAtmosphere");

	//CPixelBuffer pb;
	//pb.Init(256, 256, 1);
	//pb.MakeGlow2D(40.0f, 0.1f);

	shaderSupported = true;
	if (shaderSupported) {
		//TODO: this stuff

		shader = makeShaderProgramFromFile({ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, { "../work/res/shaders/vert_simpleTexture.vert", "../work/res/shaders/frag_simpleTexture.frag" });
		
		glUseProgram(shader);

		uniformExposure = 0.0017f;
		uniformDecay = 1.0f;
		uniformDensity = 0.84f;
		uniformWeight = 5.65f;

		localLight = glGetUniformLocation(shader, "lightPositionOnScreen");
		glUniform2f(localLight, uniformLightX, uniformLightY);
		localExposure = glGetUniformLocation(shader, "exposure");
		glUniform1f(localExposure, uniformExposure);
		localDecay = glGetUniformLocation(shader, "decay");
		glUniform1f(localDecay, uniformDecay);
		localDensity = glGetUniformLocation(shader, "density");
		glUniform1f(localDensity, uniformDensity);
		localWeight = glGetUniformLocation(shader, "weight");
		glUniform1f(localWeight, uniformWeight);
		localTexture = glGetUniformLocation(shader, "myTexture");
		glUniform1f(localTexture, screenCopyTextureId);
		/*
		cout << "Light: " << localLight << endl;
		cout << "Exposure: " << localExposure << endl;
		cout << "Decay: " << localDecay << endl;
		cout << "Density: " << localDensity << endl;
		cout << "Weight: " << localWeight << endl;
		cout << "Texture: " << localTexture << endl;
		*/
		//GLuint atmosshader = makeShaderProgramFromFile({ GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, { "../work/res/shaders/GroundFromAtmosphere.vert", "../work/res/shaders/GroundFromAtmosphere.frag" });

		glUseProgram(0);
	}

}

void Engine::run()
	{
	
}

void Engine::update() 
{

}

void Engine::render()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//glColor4f(1, 1, 1, 1);//here is the background colour, which I need to replace


	glViewport(0, 0, renderWidth / OFF_SCREEN_RENDER_RATIO, renderHeight / OFF_SCREEN_RENDER_RATIO);

	if (stage >= 1){
		//STEP 1 ------------------------
		//Draw objects black with white light
		glDisable(GL_TEXTURE_2D);
		glPushMatrix();
			glEnable(GL_COLOR_MATERIAL);
				glTranslatef(light->lightPos[0], light->lightPos[1], light->lightPos[2]);
				light->render();
			glDisable(GL_COLOR_MATERIAL);
		glPopMatrix();

		getLightScreenCoord();
	
		//glUseProgram(0);
	
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		glColor4f(0, 0, 0, 1);// no need for this when using geometry main.
		glPushMatrix();
			//No textures + all black
			geometryMain->setTexturesOn(false);
			geometryMain->setColorOn(false);
			geometryMain->renderGeometry();
		glPopMatrix();

		copyFrameBufferToTexture();
	
		glViewport(0, 0, renderWidth, renderHeight);
		//glEnable(GL_TEXTURE_2D); // no need for this...
	
		if (stage >= 2){
			//STEP 2 ------------------------
			//Draw scene with no light scattering
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glColor4f(1, 1, 1, 1); //maybe?
			glPushMatrix();
				geometryMain->setColorOn(true);
				geometryMain->setTexturesOn(true); //true
				geometryMain->renderGeometry();
				//geometryMain->setTexturesOn(false); //false, no need for this i think (99% sure)
			glPopMatrix();
	
			if (stage >= 3){
				//STEP 3 ------------------------
				//Paint light scattering effect
				if (stage >= 4) {
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				}
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrtho(-renderWidth / 2, renderWidth / 2, -renderHeight / 2, renderHeight / 2, 0, 50000.0);
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				glClear(GL_DEPTH_BUFFER_BIT);

				glActiveTexture(GL_TEXTURE_2D);	// GL_TEXTURE9 ????? Welp, ok lol
				glBindTexture(GL_TEXTURE_2D, screenCopyTextureId);

				//update values
				glUseProgram(shader);
				glUniform2f(localLight, uniformLightX, uniformLightY); //mostly just this one
				glUniform1f(localExposure, uniformExposure);
				glUniform1f(localDecay, uniformDecay);
				glUniform1f(localDensity, uniformDensity);
				glUniform1f(localWeight, uniformWeight);
				glUniform1i(localTexture, 0);

				glEnable(GL_TEXTURE_2D);
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
	
				glUseProgram(0);
	

				//glDisable(GL_BLEND);
				//glDisable(GL_TEXTURE9);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDisable(GL_LIGHTING);
			}
		}
	}
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

	uniformLightX = windowX / ((float)renderWidth / OFF_SCREEN_RENDER_RATIO);
	uniformLightY = windowY / ((float)renderHeight / OFF_SCREEN_RENDER_RATIO);
}

void Engine::createScreenCopyTexture()
{
	int width = renderWidth;
	int height = renderHeight;

	char* emptyData = (char*)malloc(width * height * 3 * sizeof(char)); //allocate memory for textures (*3 for rgb)
	memset(emptyData, 0, width * height * 3 * sizeof(char)); //set memory

	glActiveTexture(GL_TEXTURE9);
	glGenTextures(1, &screenCopyTextureId);
	glBindTexture(GL_TEXTURE_2D, screenCopyTextureId);	//bind texture name
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, emptyData); //specify texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//use lerp for magnification and minification (below)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	free(emptyData);	//release memory;
}

void Engine::copyFrameBufferToTexture() 
{
	glBindTexture(GL_TEXTURE_2D, screenCopyTextureId);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, renderWidth, renderHeight);
}



