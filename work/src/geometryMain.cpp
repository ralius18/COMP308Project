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
	objects[0] = new Geometry("../work/res/assets/sphere.obj", 4);
	objects[1] = new Geometry("../work/res/assets/box.obj", 2);
	objects[2] = new Geometry("../work/res/assets/teapot.obj", 1);
	//objects[3] = new Geometry("../work/res/assets/sphere.obj", 1);
}

void GeometryMain::loadTextures() {

	texturesSize = 8; //The amount of textures we are loading in
	textures = new GLuint[texturesSize];

	//Load in images
	Image texBrick("../work/res/textures/brick.jpg");
	Image texWood("../work/res/textures/wood.jpg");
	Image texCloud("../work/res/textures/background.png");
	Image texMetal1("../work/res/textures/metal-1.jpg");
	Image texFLight("../work/res/textures/fake-light.jpg");
	Image texRust1("../work/res/textures/trak_rustdecal2.tga");
	Image texFLight2("../work/res/textures/fake-light2.jpg");
	Image texMark1("../work/res/textures/mark1.jpg");
	Image texRust2("../work/res/textures/trak_rustdecal3.tga");

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

	glBindTexture(GL_TEXTURE_2D, textures[background]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texCloud.w, texCloud.h, texCloud.glFormat(), GL_UNSIGNED_BYTE, texCloud.dataPointer());

	glBindTexture(GL_TEXTURE_2D, textures[metal1]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texMetal1.w, texMetal1.h, texMetal1.glFormat(), GL_UNSIGNED_BYTE, texMetal1.dataPointer());

	glBindTexture(GL_TEXTURE_2D, textures[fake_light]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texFLight.w, texFLight.h, texFLight.glFormat(), GL_UNSIGNED_BYTE, texFLight.dataPointer());

	glBindTexture(GL_TEXTURE_2D, textures[rust1]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texRust1.w, texRust1.h, texRust1.glFormat(), GL_UNSIGNED_BYTE, texRust1.dataPointer());

	glBindTexture(GL_TEXTURE_2D, textures[fake_light2]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texFLight2.w, texFLight2.h, texFLight2.glFormat(), GL_UNSIGNED_BYTE, texFLight2.dataPointer());

	glBindTexture(GL_TEXTURE_2D, textures[mark]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texMark1.w, texMark1.h, texMark1.glFormat(), GL_UNSIGNED_BYTE, texMark1.dataPointer());
	
	glBindTexture(GL_TEXTURE_2D, textures[rust2]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texRust2.w, texRust2.h, texRust2.glFormat(), GL_UNSIGNED_BYTE, texRust2.dataPointer());

}

void GeometryMain::renderGeometry() {

	if (textures_enabled) {
		// Enable Drawing texures
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE1);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE2);
		glEnable(GL_TEXTURE_2D);
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
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, textures[brick]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textures[fake_light]);
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
		glMultiTexCoord2f(GL_TEXTURE2, sqXs[i], sqZs[i]);
		glVertex3f(sqXs[i] * size, -2, sqZs[i] * size);
	}
	glEnd();

	glPopMatrix();

	//Draw sphere
	if (color_enabled)
		glColor3f(1.0f, 0.9f, 0.8f); //Metalic Bronze color
	if (textures_enabled) {
		glActiveTexture(GL_TEXTURE0);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textures[metal1]);
		glActiveTexture(GL_TEXTURE1);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textures[fake_light2]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textures[rust1]);
	}
	glPushMatrix();
	glTranslatef(5, 2, 0);
	glScalef(2, 2, 2);
	objects[sphere]->renderGeometry();
	vec3 C = {0,0,0};
	vec3 D = {0,0,0};
	sphereminmax.clear();
	sphereminmax.push_back(C);
	sphereminmax.push_back(D);
	sphereminmax = (objects[sphere]->collision());

	glPopMatrix();

	//Draw second shpere
	if (color_enabled)
		glColor3f(0.8f, 0.8f, 0.8f); //grey/white
	if (textures_enabled) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[background]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textures[rust2]);
	}

	glPushMatrix();
	glTranslatef(5, 2, 10);
	glRotatef(160, 0, 1, 0);
	glScalef(2, 2, 2);
	objects[sphere]->renderGeometry();
	vec3 T = {0,0,0};
	vec3 M = {0,0,0};
	sphereminmax.clear();
	sphereminmax.push_back(T);
	sphereminmax.push_back(M);
	sphere2minmax = objects[sphere]->collision();
	glPopMatrix();

	//Draw box
	if (color_enabled)
		glColor3f(0.75f, 0.0f, 0.0f); //Plastic Red
	if (textures_enabled) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[mark]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[fake_light2]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glPushMatrix();
	glTranslatef(10, -1, -10);
	glRotatef(10, 0, 1, 0);
	glScalef(0.5, 0.5, 0.5);
	objects[box]->renderGeometry();
	vec3 A = {0,0,0};
	vec3 B = {0,0,0};
	boxminmax.clear();
	boxminmax.push_back(A);
	boxminmax.push_back(B);
	boxminmax = (objects[box]->collision());
	glPopMatrix();

	//Draw box2
	if (color_enabled)
		glColor3f(0.0f, 0.75f, 0.0f); //Plastic Green
	glPushMatrix();
	glTranslatef(6, -1, -11);
	glRotatef(40, 0, 1, 0);
	glScalef(0.5, 0.5, 0.5);
	objects[box]->renderGeometry();
	vec3 G = {0,0,0};
	vec3 H = {0,0,0};
	box2minmax.clear();
	box2minmax.push_back(G);
	box2minmax.push_back(H);
	box2minmax = objects[box]->collision();
	glPopMatrix();

	//Draw box3
	if (color_enabled)
		glColor3f(0.0f, 0.0f, 0.75f); //Plastic Blue
	glPushMatrix(); 
	glTranslatef(9, -1, -14);
	glRotatef(-60, 0, 1, 0);
	glScalef(0.5, 0.5, 0.5);
	objects[box]->renderGeometry();
	vec3 Q = {0,0,0};
	vec3 R = {0,0,0};
	box3minmax.clear();
	box3minmax.push_back(Q);
	box3minmax.push_back(R);
	box3minmax = objects[box]->collision();
	glPopMatrix();

	//Draw teapot
	if (color_enabled)
		glColor3f(0.8f, 0.8f, 0.8f); //White
	if (textures_enabled) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[background]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textures[mark]);
	}
	glPushMatrix();
	glTranslatef(-10, -2, 6);
	glRotatef(-50, 0, 1, 0);
	objects[teapot]->renderGeometry();
	vec3 E = {0,0,0};
	vec3 F = {0,0,0};
	sphereminmax.clear();
	sphereminmax.push_back(E);
	sphereminmax.push_back(F);
	teapotminmax = objects[teapot]->collision();
	glPopMatrix();

	// Cleanup
	if (textures_enabled) {
		//clearing all the gl_textures that i've used
		for (int i = 0; i <= 2; i++) {
			glActiveTexture(GL_TEXTURE0+i);
			glBindTexture(GL_TEXTURE_2D, 0);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glDisable(GL_TEXTURE_2D);
		}
		glActiveTexture(GL_TEXTURE1);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, 0);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, 0);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
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