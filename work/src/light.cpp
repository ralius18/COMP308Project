#include "light.hpp"

using namespace cgra;

Light::Light(void)
{
	lightPos[0] = -500.0f;
	lightPos[1] = 500.0f;
	lightPos[2] = 0.0f;
	lightPos[3] = 1.0f;

	diffuse[0] = 1.0f;
	diffuse[1] = 1.0f;
	diffuse[2] = 1.0f;

	materialDiffuse[0] = 1.0f;
	materialDiffuse[1] = 1.0f;
	materialDiffuse[2] = 1.0f;
	materialDiffuse[3] = 1.0f;

	displayList = glGenLists(1);
	glNewList(displayList, GL_COMPILE);

		glColor4f(diffuse[0], diffuse[1], diffuse[2], 1);
		cgraSphere(45, 40, 40);

	glEndList();
}

Light::~Light(void)
{

}

void Light::apply()
{

}

void Light::render()
{
	glCallList(displayList);
}