//---------------------------------------------------------------------------
//
// Copyright (c) 2016 Taehyun Rhee, Joshua Scott, Ben Allen
//
// This software is provided 'as-is' for assignment of COMP308 in ECS,
// Victoria University of Wellington, without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#include <cmath>
#include <iostream> // input/output streams
#include <fstream>  // file streams
#include <sstream>  // string streams
#include <string>
#include <stdexcept>
#include <vector>

#include "cgra_math.hpp"
#include "geometry.hpp"
#include "opengl.hpp"
#include "geometryMain.hpp"
#include "simple_image.hpp"

using namespace std;
using namespace cgra;

GeometryMain::GeometryMain() {
	//Loading will be mostly hard coded!!! Check methods for alteration.
	loadObjects();
	loadTextures();
	//Render method is also has hard coded translation/rot/scaling/etc.
}

GeometryMain::~GeometryMain() { }

void GeometryMain::loadObjects() {

	objects = new Geometry*[3];

	//Args are sting for the file, int for how much the texture spreads on the object
	objects[0] = new Geometry("../work/res/assets/sphere.obj", 1);
	objects[1] = new Geometry("../work/res/assets/box.obj", 8);
	//objects[2] = new Geometry("../work/res/assets/teapot.obj", 1);
}

void GeometryMain::loadTextures() {

	texturesSize = 5; //The amount of textures we are loading in
	textures = new GLuint[texturesSize];

	//Load in images
	Image texBrick("../work/res/textures/brick.jpg");
	Image texWood("../work/res/textures/wood.jpg");
	Image texCloud("../work/res/textures/cloud.jpg");
	Image texCheckRed("../work/res/textures/red-checks.png");
	Image texCheckGreen("../work/res/textures/green-checks.png");

	//Generate our textures array.
	glGenTextures(texturesSize, textures); // Generate texture IDs

	// Setup sampling strategies
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Use slot 0, need to use GL_TEXTURE1 ... etc if using more than one texture PER OBJECT
	//glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, textures[brick]); // Bind our first texture as a 2D texture
	// Finnaly, actually fill the data into our texture
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texBrick.w, texBrick.h, texBrick.glFormat(), GL_UNSIGNED_BYTE, texBrick.dataPointer());

	glBindTexture(GL_TEXTURE_2D, textures[wood]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texWood.w, texWood.h, texWood.glFormat(), GL_UNSIGNED_BYTE, texWood.dataPointer());

	glBindTexture(GL_TEXTURE_2D, textures[cloud]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texCloud.w, texCloud.h, texCloud.glFormat(), GL_UNSIGNED_BYTE, texCloud.dataPointer());

	glBindTexture(GL_TEXTURE_2D, textures[3]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texCheckRed.w, texCheckRed.h, texCheckRed.glFormat(), GL_UNSIGNED_BYTE, texCheckRed.dataPointer());
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, textures[4]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texCheckGreen.w, texCheckGreen.h, texCheckGreen.glFormat(), GL_UNSIGNED_BYTE, texCheckGreen.dataPointer());


}

void GeometryMain::renderGeometry() {

	if (textures_enabled) {
		// Enable Drawing texures
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		// Use Texture as the color
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
	//Enable using colour as a material.
	glEnable(GL_COLOR_MATERIAL);
	//Default color used when color is disabled (color_enabled == false)
	glColor3f(0, 0, 0);

	//Draw floor
	if (color_enabled)
		glColor3f(0.3f, 0.3f, 0.3f);
	if (textures_enabled) {
		glActiveTexture(GL_TEXTURE0);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//Texture for the floor because putting it anywhere else won't work...
		glBindTexture(GL_TEXTURE_2D, textures[brick]);
		glActiveTexture(GL_TEXTURE1);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textures[cloud]);
	}

	float sqXs[4] = { 0, 1, 1, 0 };
	float sqZs[4] = { 0, 0, 1, 1 };
	int size = 40; //Length (or width) of the square
	int textureSpread = 6; //How much the texture repeats itself over the square.
	glPushMatrix();
	glTranslatef(-size/2, 0, -size/2);

	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++) {
		glNormal3f(sqXs[i] * size, -2, sqZs[i] * size);
		glMultiTexCoord2f(GL_TEXTURE0, sqXs[i]*textureSpread, sqZs[i]*textureSpread);
		glMultiTexCoord2f(GL_TEXTURE1, sqXs[i], sqZs[i]);
		glVertex3f(sqXs[i] * size, -2, sqZs[i] * size);
	}
	glEnd();

	glPopMatrix();
	glPushMatrix();

	//Draw sphere
	if (color_enabled)
		glColor3f(0.8f, 0.3f, 0.0f); //Metalic Bronze color
	if (textures_enabled) {
		glActiveTexture(GL_TEXTURE0);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textures[cloud]);
		glActiveTexture(GL_TEXTURE1);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	objects[sphere]->renderGeometry();
	sphereminmax = objects[sphere]->collision();

	//Draw box
	glPushMatrix();
	glTranslatef(10, 0, -10);
	if (color_enabled)
		glColor3f(1.0f, 1.0f, 1.0f); //Dark Red
	if (textures_enabled) {
		glActiveTexture(GL_TEXTURE0);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textures[3]);
		glActiveTexture(GL_TEXTURE1);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textures[4]);
	}
	objects[box]->renderGeometry();
	boxminmax = objects[box]->collision();
	glPopMatrix();

	//Draw teapot
	glPushMatrix();
	glTranslatef(4, -2, -6);
	glRotatef(-50, 0, 1, 0);
	if (color_enabled)
		glColor3f(0.6f, 0.6f, 0.6f); //Dark Red
	if (textures_enabled) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textures[brick]);
	}
	//objects[teapot]->renderGeometry();
	teapotminmax = objects[teapot]->collision();
	glPopMatrix();

	// Cleanup
	glPopMatrix();
	if (textures_enabled) {
		//clearing all the gl_textures that i've used
		for (int i = 0; i <= 5; i++) {
			glActiveTexture(GL_TEXTURE0+i);
			glBindTexture(GL_TEXTURE_2D, 0);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glDisable(GL_TEXTURE_2D);
		}
		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glActiveTexture(GL_TEXTURE0);
		
	}
	glDisable(GL_COLOR_MATERIAL);
	glColor3f(0, 0, 0);
}

void GeometryMain::toggleTextures() {
	textures_enabled = !textures_enabled;
	//if(textures_enabled) cout << "Textures are: Enabled" << endl;
	//else cout << "Textures are: Disabled" << endl;
}

void GeometryMain::toggleColor() {
	color_enabled = !color_enabled;
	//if (color_enabled) cout << "Colors are: Enabled" << endl;
	//else cout << "Colors are: Disabled" << endl;
}

//Set to true to load in color, false else (everything black).
void GeometryMain::setColorOn(bool c) {
	color_enabled = c;
	//cout << "" << endl;
}

void GeometryMain::setTexturesOn(bool t) {
	textures_enabled = t;
	//cout << "" << endl;
}