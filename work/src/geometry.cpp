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

using namespace std;
using namespace cgra;


Geometry::Geometry(string filename) {
	m_filename = filename;
	object obj;
	readOBJ(filename,&obj);
	if (obj.m_triangles.size() > 0) {
		createDisplayListPoly(&obj);
	}
	objects.push_back(obj);
}


Geometry::Geometry() { 

	object obj;

	readOBJ("../work/res/assets/box.obj", &obj);
	if (obj.m_triangles.size() > 0) {
		createDisplayListPoly(&obj);
	}
	objects.push_back(obj);
}


void Geometry::readOBJ(string filename, struct object* obj) {
	
	obj->m_filename = filename;

	// Make sure our geometry information is cleared
	obj->m_points.clear();
	obj->m_uvs.clear();
	obj->m_normals.clear();
	obj->m_triangles.clear();

	// Load dummy points because OBJ indexing starts at 1 not 0
	obj->m_points.push_back(vec3(0,0,0));
	obj->m_uvs.push_back(vec2(0,0));
	obj->m_normals.push_back(vec3(0,0,1));


	ifstream objFile(filename);

	if(!objFile.is_open()) {
		cerr << "Error reading " << filename << endl;
		throw runtime_error("Error :: could not open file.");
	}

	cout << "Reading file " << filename << endl;
	
	// good() means that failbit, badbit and eofbit are all not set
	while(objFile.good()) {

		// Pull out line from file
		string line;
		std::getline(objFile, line);
		istringstream objLine(line);

		// Pull out mode from line
		string mode;
		objLine >> mode;

		// Reading like this means whitespace at the start of the line is fine
		// attempting to read from an empty string/line will set the failbit
		if (!objLine.fail()) {

			if (mode == "v") {
				vec3 v;
				objLine >> v.x >> v.y >> v.z;
				obj->m_points.push_back(v);

			} else if(mode == "vn") {
				vec3 vn;
				objLine >> vn.x >> vn.y >> vn.z;
				obj->m_normals.push_back(vn);

			} else if(mode == "vt") {
				vec2 vt;
				objLine >> vt.x >> vt.y;
				obj->m_uvs.push_back(vt);

			} else if(mode == "f") {

				vector<vertex> verts;
				while (objLine.good()) {
					vertex v;

					//-------------------------------------------------------------
					// [Assignment 1] :
					// Modify the following to parse the bunny.obj-> It has no uv
					// coordinates so each vertex for each face is in the format
					// v//vn instead of the usual v/vt/vn.
					//
					// Modify the following to parse the dragon.obj-> It has no
					// normals or uv coordinates so the format for each vertex is
					// v instead of v/vt/vn or v//vn.
					//
					// Hint : Check if there is more than one uv or normal in
					// the uv or normal vector and then parse appropriately.
					//-------------------------------------------------------------

					// Assignment code (assumes you have all of v/vt/vn for each vertex)
					objLine >> v.p;		// Scan in position index
					if (obj->m_normals.size() > 1) {
						objLine.ignore(1);	// Ignore the '/' character
						if (obj->m_uvs.size() > 1) {
							objLine >> v.t;		// Scan in uv (texture coord) index
						}
						objLine.ignore(1);	// Ignore the '/' character
						objLine >> v.n;		// Scan in normal index
					}
					verts.push_back(v);
				}

				// IFF we have 3 verticies, construct a triangle
				if(verts.size() == 3){
					triangle tri;
					tri.v[0] = verts[0];
					tri.v[1] = verts[1];
					tri.v[2] = verts[2];
					obj->m_triangles.push_back(tri);

				}
			}
		}
	}

	cout << "Reading OBJ file is DONE." << endl;
	cout << obj->m_points.size()-1 << " points" << endl;
	cout << obj->m_uvs.size()-1 << " uv coords" << endl;
	cout << obj->m_normals.size()-1 << " normals" << endl;
	cout << obj->m_triangles.size() << " faces" << endl;


	// If we didn't have any normals, create them
	if (obj->m_normals.size() <= 1) createNormals(obj);
}


//-------------------------------------------------------------
// [Assignment 1] :
// Fill the following function to populate the normals for 
// the model currently loaded. Compute per face normals
// first and get that working before moving onto calculating
// per vertex normals.
//-------------------------------------------------------------
void Geometry::createNormals(struct object* obj) {
	// YOUR CODE GOES HERE
	// ...
	//
	// Calculating per face normals and applying to vertices
	for (int i = 0; i < obj->m_triangles.size(); i++) {
		//calculate cross product (face normal): (p2-p1)x(p3-p1)
		vec3 u = obj->m_points[obj->m_triangles[i].v[1].p] - obj->m_points[obj->m_triangles[i].v[0].p];	//calculate 1st vector
		vec3 v = obj->m_points[obj->m_triangles[i].v[2].p] - obj->m_points[obj->m_triangles[i].v[0].p];	//calculate 2nd vector
		vec3 n = cross(u, v);	//calculate cross product from two vectors
		obj->m_normals.push_back(n);		//add normal to list
		obj->m_triangles[i].v[0].n = obj->m_normals.size()-1;	//apply normal to vertex

		//(p3 - p2)x(p1 - p2)
		u = obj->m_points[obj->m_triangles[i].v[2].p] - obj->m_points[obj->m_triangles[i].v[1].p];	//calculate 1st vector
		v = obj->m_points[obj->m_triangles[i].v[0].p] - obj->m_points[obj->m_triangles[i].v[1].p];	//calculate 2nd vector
		n = cross(u, v);	//calculate cross product from two vectors
		obj->m_normals.push_back(n);		//add normal to list
		obj->m_triangles[i].v[1].n = obj->m_normals.size() - 1;	//apply normal to vertex

		//(p1 - p3)x(p2 - p3)
		u = obj->m_points[obj->m_triangles[i].v[0].p] - obj->m_points[obj->m_triangles[i].v[2].p];	//calculate 1st vector
		v = obj->m_points[obj->m_triangles[i].v[1].p] - obj->m_points[obj->m_triangles[i].v[2].p];	//calculate 2nd vector
		n = cross(u, v);	//calculate cross product from two vectors
		obj->m_normals.push_back(n);		//add normal to list
		obj->m_triangles[i].v[2].n = obj->m_normals.size() - 1;	//apply normal to vertex
	}
}


//-------------------------------------------------------------
// [Assignment 1] :
// Fill the following function to create display list
// of the obj file to show it as wireframe model
//-------------------------------------------------------------
void Geometry::createDisplayListPoly(struct object* obj) {
	// Delete old list if there is one
	obj->m_displayListPoly = GLuint(0);
	if (obj->m_displayListPoly) glDeleteLists(obj->m_displayListPoly, 2);

	// Create a new list
	cout << "Creating Poly Geometry" << endl;
	obj->m_displayListPoly = glGenLists(2);
	glNewList(obj->m_displayListPoly, GL_COMPILE);

	// YOUR CODE GOES HERE
	// ...
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < obj->m_triangles.size(); i++) {	//loop through all triangles
		for (int j = 0; j < 3; j++) {	//loop through each vertex of the triangle
			glNormal3f(obj->m_normals[obj->m_triangles[i].v[j].n].x, obj->m_normals[obj->m_triangles[i].v[j].n].y, obj->m_normals[obj->m_triangles[i].v[j].n].z);	//add the normal
			glTexCoord2f(obj->m_uvs[obj->m_triangles[i].v[j].t].x*4, obj->m_uvs[obj->m_triangles[i].v[j].t].y*4);	//add the uv co-ord
			glVertex3f(obj->m_points[obj->m_triangles[i].v[j].p].x, obj->m_points[obj->m_triangles[i].v[j].p].y, obj->m_points[obj->m_triangles[i].v[j].p].z);	//add the vertex
		}
	}
	glEnd();
	glFlush();
	
	glEndList();
	cout << "Finished creating Poly Geometry" << endl;
}


void Geometry::renderGeometry(GLuint g_texture, GLuint g_texture1) {

	for (object obj : objects) {
		glCallList(obj.m_displayListPoly);
	}

}