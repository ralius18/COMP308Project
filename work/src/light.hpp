#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "math.h"
#include "geometry.hpp"
#include "opengl.hpp"
#include "cgra_geometry.hpp"



using namespace std;

class Light
{
public:
	Light(void);
	~Light(void);

	void apply(void);
	void render(void);
	void update(void);

	GLfloat lightPos[4];
	GLfloat diffuse[3];
	GLfloat ambient[4];
	GLfloat specular[4];

	GLfloat materialDiffuse[4];

	GLuint displayList;
};