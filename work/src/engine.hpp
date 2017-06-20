#pragma once

#include "light.hpp"
#include "geometryMain.hpp"


class Engine
{
public:
	Engine(GLFWwindow*);

	void mainLoop();
	void render();
	void run();
	void update();

	static Engine* engine;
	GeometryMain* geometryMain;
	GLFWwindow* window;

	Light* light;

	int stage = 3;

	//offscreen rendering
	bool shaderSupported;
	GLuint shader;

	float uniformLightX;
	float uniformLightY;
	float uniformExposure;
	float uniformDecay;
	float uniformDensity;
	float uniformWeight;

	int m_nSamples;
	float m_Kr, m_Kr4PI;
	float m_Km, m_Km4PI;
	float m_ESun;
	float m_g;
	float m_fExposure;


	float m_fInnerRadius;
	float m_fOuterRadius;
	float m_fScale;
	float m_fWavelength[3];
	float m_fWavelength4[3];
	float m_fRayleighScaleDepth;
	float m_fMieScaleDepth;

	//CPBuffer m_pBuffer;
	//CPixelBuffer m_pbOpticalDepth;

	//CShaderObject m_shSkyFromSpace;
	//CShaderObject m_shSkyFromAtmosphere;
	//CShaderObject m_shGroundFromSpace;
	//CShaderObject m_shGroundFromAtmosphere;
	//CShaderObject m_shSpaceFromSpace;
	//CShaderObject m_shSpaceFromAtmosphere;
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
	void scatteringRender(float cam_x, float cam_y, float cam_z);

};