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
	sphere = new Geometry("../work/res/assets/sphere.obj");
}

void GeometryMain::loadTextures() {

	texturesSize = 2; //The amount of textures we are loading in
	textures = new GLuint[texturesSize];

	//Load in images
	Image texBrick("../work/res/textures/brick.jpg");
	Image texWood("../work/res/textures/wood.jpg");

	//Generate our textures array.
	glGenTextures(texturesSize, textures); // Generate texture IDs

	// Setup sampling strategies
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Use slot 0, need to use GL_TEXTURE1 ... etc if using more than one texture PER OBJECT
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, textures[0]); // Bind our first texture as a 2D texture
	// Finnaly, actually fill the data into our texture
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texBrick.w, texBrick.h, texBrick.glFormat(), GL_UNSIGNED_BYTE, texBrick.dataPointer());

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texWood.w, texWood.h, texWood.glFormat(), GL_UNSIGNED_BYTE, texWood.dataPointer());
}

void GeometryMain::renderGeometry() {
	
	//Enable using colour as a material.
	//glEnable(GL_COLOR_MATERIAL);
	// Enable Drawing texures
	glEnable(GL_TEXTURE_2D);
	// Use Texture as the color
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	// Set the location for binding the texture
	glActiveTexture(GL_TEXTURE0);
	// Bind the texture
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, textures[1]);
	glColor3f(0.6f, 0.2f, 0.0f);

	//Draw all our Geometry's (models/objects)
	//glColor3f(0.6f, 0.2f, 0.0f); //Metalic Bronze color
	glPushMatrix();
	//glTranslatef(0.0f, -1.0f, 0.0f);
	sphere->renderGeometry();

	// Cleanup
	glPopMatrix();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	//glDisable(GL_COLOR_MATERIAL);
}