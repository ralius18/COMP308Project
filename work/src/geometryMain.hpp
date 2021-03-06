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

#pragma once

#include <cmath>
#include <iostream>
#include <string>

#include "opengl.hpp"

class GeometryMain {
	
private:

	int texturesSize;
	GLuint *textures;
	Geometry **objects = nullptr;
	enum objectNames { sphere, box, teapot };

	bool textures_enabled = false;
	bool color_enabled = true;

	void loadObjects();
	void loadTextures();
	enum textureNames { brick, wood, background, metal1, fake_light, rust1, fake_light2, mark, rust2 };

	

public:
	GeometryMain();
	~GeometryMain();

	std::vector<cgra::vec3> sphereminmax;
	std::vector<cgra::vec3> boxminmax;
	std::vector<cgra::vec3> teapotminmax;
	std::vector<cgra::vec3> sphere2minmax;
	std::vector<cgra::vec3> box2minmax;
	std::vector<cgra::vec3> box3minmax;

	void renderGeometry();
	void toggleTextures();
	void toggleColor();
	void setColorOn(bool c);
	void setTexturesOn(bool t);
};
