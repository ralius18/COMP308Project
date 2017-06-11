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
	Geometry *sphere;
	void loadObjects();
	void loadTextures();

public:
	GeometryMain();
	~GeometryMain();

	void renderGeometry();
};