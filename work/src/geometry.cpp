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

Geometry::Geometry(string filename, int textureSpread) {
	txtrSprd = textureSpread;

	m_filename = filename;
	readOBJ(filename);
	if (m_triangles.size() > 0) {
		createDisplayListPoly();
		//createDisplayListWire();
	}
}


Geometry::~Geometry() { }

void Geometry::readOBJ(string filename) {

	// Make sure our geometry information is cleared
	m_points.clear();
	m_uvs.clear();
	m_normals.clear();
	m_triangles.clear();

	// Load dummy points because OBJ indexing starts at 1 not 0
	m_points.push_back(vec3(0,0,0));
	m_uvs.push_back(vec2(0,0));
	m_normals.push_back(vec3(0,0,1));


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
				m_points.push_back(v);

			} else if(mode == "vn") {
				vec3 vn;
				objLine >> vn.x >> vn.y >> vn.z;
				m_normals.push_back(vn);

			} else if(mode == "vt") {
				vec2 vt;
				objLine >> vt.x >> vt.y;
				m_uvs.push_back(vt);

			} else if(mode == "f") {

				vector<vertex> verts;
				while (objLine.good()){
					vertex v;

					//-------------------------------------------------------------
					// [Assignment 1] :
					// Modify the following to parse the bunny.obj. It has no uv
					// coordinates so each vertex for each face is in the format
					// v//vn instead of the usual v/vt/vn.
					//
					// Modify the following to parse the dragon.obj. It has no
					// normals or uv coordinates so the format for each vertex is
					// v instead of v/vt/vn or v//vn.
					//
					// Hint : Check if there is more than one uv or normal in
					// the uv or normal vector and then parse appropriately.
					//-------------------------------------------------------------

					// Assignment code (assumes you have all of v/vt/vn for each vertex)
					objLine >> v.p;		// Scan in position index
					if (m_normals.size() > 1) { //if we are not looking at dragon (which has no '/'s)
						objLine.ignore(1);	// Ignore the '/' character
						if (m_uvs.size() > 1) { //if there are usvs, do the thing
							objLine >> v.t;		// Scan in uv (texture coord) index
						}
						objLine.ignore(1);	// Ignore the '/' character

						objLine >> v.n;		// Scan in normal index
					}
					verts.push_back(v);
				}
				//cout << verts.size();
				// IFF we have 4 verticies, construct a square
				if(verts.size() >= 3){
					triangle tri;
					//cout << m_triangles.size() << endl;
					tri.v[0] = verts[0];
					tri.v[1] = verts[1];
					tri.v[2] = verts[2];
					m_triangles.push_back(tri);
					//cout << m_triangles.size() << endl;
				}
			}
		}
	}

	cout << "Reading OBJ file is DONE." << endl;
	cout << m_points.size()-1 << " points" << endl;
	cout << m_uvs.size()-1 << " uv coords" << endl;
	cout << m_normals.size()-1 << " normals" << endl;
	cout << m_triangles.size() << " faces" << endl;


	// If we didn't have any normals, create them
	if (m_normals.size() <= 1) createNormals();
}


//-------------------------------------------------------------
// [Assignment 1] :
// Fill the following function to populate the normals for
// the model currently loaded. Compute per face normals
// first and get that working before moving onto calculating
// per vertex normals.
//-------------------------------------------------------------
void Geometry::createNormals() {
	// YOUR CODE GOES HERE


	//going through all the vertecies in all the triangles
	int i;
	for (i = 0; i < m_triangles.size(); i++) {

		triangle currentSquare = m_triangles[i];

		vertex v1 = currentSquare.v[0];
		vertex v2 = currentSquare.v[1];
		vertex v3 = currentSquare.v[2];
		//vertex v4 = currentSquare.v[3];

		vec3 vecU1 = m_points[v2.p] - m_points[v1.p];
		vec3 vecV1 = m_points[v3.p] - m_points[v1.p];
		vec3 norm1 = cross(vecU1, vecV1); //the normal for v1

		vec3 vecU2 = m_points[v3.p] - m_points[v2.p];
		vec3 vecV2 = m_points[v1.p] - m_points[v2.p];
		vec3 norm2 = cross(vecU2, vecV2); //the normal for v2

		vec3 vecU3 = m_points[v1.p] - m_points[v3.p];
		vec3 vecV3 = m_points[v2.p] - m_points[v3.p];
		vec3 norm3 = cross(vecU3, vecV3); //the normal for v3

		//now we store it in m_triangles

		m_normals.push_back(norm1);
		//m_triangles[i].v[0].n = m_normals.size()-1;
		m_normals.push_back(norm2);
		//m_triangles[i].v[1].n = m_normals.size()-1;
		m_normals.push_back(norm3);
		//m_triangles[i].v[2].n = m_normals.size()-1;
	}
}


//-------------------------------------------------------------
// [Assignment 1] :
// Fill the following function to create display list
// of the obj file to show it as polygon model
//-------------------------------------------------------------
void Geometry::createDisplayListPoly() {
	// Delete old list if there is one
	if (m_displayListPoly) glDeleteLists(m_displayListPoly, 1);

	// Create a new list
	cout << "Creating Poly Geometry" << endl;
	m_displayListPoly = glGenLists(1);
	glNewList(m_displayListPoly, GL_COMPILE);

	// YOUR CODE GOES HERE

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_TRIANGLES);
	createDisplayList(); //FYI: this method calls glEndList()

	cout << "Finished creating Poly Geometry" << endl;
}


//-------------------------------------------------------------
// [Assignment 1] :
// Fill the following function to create display list
// of the obj file to show it as wireframe model
//-------------------------------------------------------------
void Geometry::createDisplayListWire() {
	// Delete old list if there is one
	if (m_displayListWire) glDeleteLists(m_displayListWire, 1);

	// Create a new list
	cout << "Creating Wire Geometry" << endl;
	m_displayListWire = glGenLists(1);
	glNewList(m_displayListWire, GL_COMPILE);

	// YOUR CODE GOES HERE

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_TRIANGLES);
	createDisplayList(); //FYI: this method calls glEndList()

	cout << "Finished creating Wire Geometry" << endl;
}

//This method is called ONLY in createDisplayListPoly and createDisplayListWire, this method creates the geometry.
//createDisplayListPoly and createDisplayListWire arae wrapper methods for this method.
void Geometry::createDisplayList(){

	//go through all the verticies in all the triangles in list of triangles 'm_triangles'.

	//used for iterating through nested for loop bellow.
	int i, j;

	//triangles always have 3 verticies.
	int numberOfVerticies= 3;

	for(i = 0; i < m_triangles.size(); i++){

		//the one triangle we are currently working with.
		triangle currentSquare = m_triangles[i];

		//used to make the for loop look cleaner
		float x, y, z;
		for(j = 0; j < numberOfVerticies; j++){

			//normals
			x = m_normals[currentSquare.v[j].n].x;
			y = m_normals[currentSquare.v[j].n].y;
			z = m_normals[currentSquare.v[j].n].z;
			glNormal3f(x, y, z);

			//UVs
			x = m_uvs[currentSquare.v[j].t].x;
			y = m_uvs[currentSquare.v[j].t].y;
			glTexCoord2f(x * txtrSprd, y * txtrSprd);
			//glMultiTexCoord2f(GL_TEXTURE0, x*txtrSprd, y*txtrSprd);
			//glMultiTexCoord2f(GL_TEXTURE1, x*txtrSprd, y*txtrSprd);

			//points
			x = m_points[currentSquare.v[j].p].x;
			y = m_points[currentSquare.v[j].p].y;
			z = m_points[currentSquare.v[j].p].z;

			glVertex3f(x, y, z);
		}
	}
	glEnd();
	glEndList();
}

void Geometry::renderGeometry() {
	
	glShadeModel(GL_SMOOTH);
	glCallList(m_displayListPoly);
}

void Geometry::toggleWireFrame() {
	cout << "WARNING: toggleWireFrame() does nothing!!!" << endl;
	cout << "WARNING: toggleWireFrame() does nothing!!!" << endl;
	cout << "WARNING: toggleWireFrame() does nothing!!!" << endl;
	m_wireFrameOn = false;//!m_wireFrameOn;
}
