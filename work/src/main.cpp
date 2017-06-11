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
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include "cgra_geometry.hpp"
#include "cgra_math.hpp"
#include "simple_image.hpp"
#include "simple_shader.hpp"
#include "opengl.hpp"
#include "geometry.hpp"
#include "geometryMain.hpp"
#include "engine.hpp"

using namespace std;
using namespace cgra;

// Window
//
GLFWwindow* g_window;


// Projection values
// 
float g_fovy = 20.0;
float g_znear = 0.1;
float g_zfar = 1000.0;


// Mouse controlled Camera values
//
bool g_leftMouseDown = false;
vec2 g_mousePosition = vec2(0, 0);
float g_pitch = 0;	//rotation around X axis
float g_yaw = 0;	//rotation around Y axis
float g_zoom = 1.0;

const float mouseXsensitivity = 0.5;
const float mouseYsensitivity = 0.5;

// Key controlled Camera values

float g_cam_x = 0.0;
float g_cam_y = 0.0;
float g_cam_z = 0.0;

// Values and fields to showcase the use of shaders
// Remove when modifying main.cpp for Assignment 3
//
bool g_useShader = false;
GLuint g_texture = 0;
GLuint g_shader = 0;

//Loads and Renders objects with multi layering textures
GeometryMain *g_geometryMain = nullptr;

Engine *g_engine;

// Mouse Button callback
// Called for mouse movement event on since the last glfwPollEvents
//
void cursorPosCallback(GLFWwindow* win, double xpos, double ypos) {

	if (g_leftMouseDown) {

		vec2 mouseDiff = vec2(xpos, ypos) - g_mousePosition;

		g_yaw += mouseXsensitivity * mouseDiff.x;
		g_pitch += mouseYsensitivity * mouseDiff.y;
	}
	g_mousePosition = vec2(xpos, ypos);
	
}


// Mouse Button callback
// Called for mouse button event on since the last glfwPollEvents
//
void mouseButtonCallback(GLFWwindow *win, int button, int action, int mods) {
	//cout << "Mouse Button Callback :: button=" << button << "action=" << action << "mods=" << mods << endl;
	if (button == GLFW_MOUSE_BUTTON_LEFT)
		g_leftMouseDown = (action == GLFW_PRESS);
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (g_useShader) {
			g_useShader = false;
			cout << "Using the default OpenGL pipeline" << endl;
		}
		else {
			g_useShader = true;
			cout << "Using a shader" << endl;
		}
	}
}


// Scroll callback
// Called for scroll event on since the last glfwPollEvents
//
void scrollCallback(GLFWwindow *win, double xoffset, double yoffset) {
	// cout << "Scroll Callback :: xoffset=" << xoffset << "yoffset=" << yoffset << endl;
	// g_zoom -= yoffset * g_zoom * 0.2;
}


// Keyboard callback
// Called for every key event on since the last glfwPollEvents
//
void keyCallback(GLFWwindow *win, int key, int scancode, int action, int mods) {
	// cout << "Key Callback :: key=" << key << "scancode=" << scancode	<< "action=" << action << "mods=" << mods << endl;
	// YOUR CODE GOES HERE
	// ...
	if (action == GLFW_PRESS || GLFW_REPEAT) {
		float yRotRad;
		float speed = 0.5;
		if (key == GLFW_KEY_W) {
			yRotRad = (g_yaw / 180 * math::pi());
			g_cam_x -= sin(yRotRad) * speed;
			g_cam_z += cos(yRotRad) * speed;
		}
		if (key == GLFW_KEY_S) {
			yRotRad = (g_yaw / 180 * math::pi());
			g_cam_x += sin(yRotRad) * speed;
			g_cam_z -= cos(yRotRad) * speed;
		}
		if (key == GLFW_KEY_A) {
			yRotRad = (g_yaw / 180 * math::pi());
			g_cam_x += cos(yRotRad) * speed;
			g_cam_z += sin(yRotRad) * speed;
		}
		if (key == GLFW_KEY_D) {
			yRotRad = (g_yaw / 180 * math::pi());
			g_cam_x -= cos(yRotRad) * speed;
			g_cam_z -= sin(yRotRad) * speed;
		}
		if (key == GLFW_KEY_SPACE) {
			g_cam_y -= speed;
		}
		if (key == GLFW_KEY_LEFT_CONTROL) {
			g_cam_y += speed;
		}
	}
}


// Character callback
// Called for every character input event on since the last glfwPollEvents
//
void charCallback(GLFWwindow *win, unsigned int c) {
	// cout << "Char Callback :: c=" << char(c) << endl;
	// Not needed for this assignment, but useful to have later on
}


// Sets up where and what the light is
// Called once on start up
// 
void initLight() {
	float direction[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	float diffintensity[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffintensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	glEnable(GL_LIGHT0);
}

// An example of how to load a shader from a hardcoded location
//
void initShader() {
	// To create a shader program we use a helper function
	// We pass it an array of the types of shaders we want to compile
	// and the corrosponding locations for the files of each stage
	g_shader = makeShaderProgramFromFile({GL_VERTEX_SHADER, GL_FRAGMENT_SHADER }, { "../work/res/shaders/shaderDemo.vert", "../work/res/shaders/shaderDemo.frag" });
}


// Sets up where the camera is in the scene
// 
void setupCamera(int width, int height) {
	// Set up the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(g_fovy, width / float(height), g_znear, g_zfar);
	//gluPerspective(g_fovy, g_pitch / float(g_yaw), g_znear, g_zfar);

	// Set up the view part of the model view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//float camPos[] = { g_cam_pos_x, g_cam_pos_y, g_cam_pos_z };
	//float camLook[] = { g_cam_pos_x, g_cam_pos_y, g_cam_pos_z + 1 };
	//float camUps[] = { 0, 1.0, 0 }; //just ignore this lol, adjusts tilt i think...

	//glTranslatef(0, 0, -50 * g_zoom);
	//glRotatef(g_pitch, 1, 0, 0);
	//glRotatef(g_yaw, 0, 1, 0);

	
	glRotatef(g_pitch, 1, 0, 0);
	glRotatef(g_yaw, 0, 1, 0);
	glTranslatef(g_cam_x, g_cam_y, g_cam_z);
}


// Draw function
//
void render(int width, int height) {

	// Grey/Blueish background
	glClearColor(0.3f, 0.3f, 0.4f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Enable flags for normal rendering
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	setupCamera(width, height);


	// Without shaders
	// Uses the default OpenGL pipeline
	//
	if (!g_useShader) {

		glPushMatrix();
			g_geometryMain->renderGeometry();
			g_engine->render();
		glPopMatrix(); //To keep things tidy.

		glFlush();
	}


	// With shaders (no lighting)
	// Uses the shaders that you bind for the graphics pipeline
	//
	else {

		// Texture setup
		//
		// Enable Drawing texures
		glEnable(GL_TEXTURE_2D);
		// Set the location for binding the texture
		glActiveTexture(GL_TEXTURE0);
		// Bind the texture
		glBindTexture(GL_TEXTURE_2D, g_texture);

		// Use the shader we made
		glUseProgram(g_shader);

		// Set our sampler (texture0) to use GL_TEXTURE0 as the source
		glUniform1i(glGetUniformLocation(g_shader, "texture0"), 0);


		// Render a single square as our geometry
		// You would normally render your geometry here
		/*
		glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0, 1.0);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-5.0, -5.0, 0.0);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-5.0, 5.0, 0.0);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(5.0, 5.0, 0.0);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(5.0, -5.0, 0.0);
		glEnd();
		*/

		glBindTexture(GL_TEXTURE_2D, 0);
		glColor3f(1, 0, 0);
		cgraSphere(4);

		glFlush();

		// Unbind our shader
		glUseProgram(0);
	}


	// Disable flags for cleanup (optional)
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_NORMALIZE);
}


// Forward decleration for cleanliness (Ignore)
void APIENTRY debugCallbackARB(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, GLvoid*);


//Main program
// 
int main(int argc, char **argv) {

	// Initialize the GLFW library
	if (!glfwInit()) {
		cerr << "Error: Could not initialize GLFW" << endl;
		abort(); // Unrecoverable error
	}

	// Get the version for GLFW for later
	int glfwMajor, glfwMinor, glfwRevision;
	glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRevision);

	// Create a windowed mode window and its OpenGL context
	g_window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
	if (!g_window) {
		cerr << "Error: Could not create GLFW window" << endl;
		abort(); // Unrecoverable error
	}

	// Make the g_window's context is current.
	// If we have multiple windows we will need to switch contexts
	glfwMakeContextCurrent(g_window);



	// Initialize GLEW
	// must be done after making a GL context current (glfwMakeContextCurrent in this case)
	glewExperimental = GL_TRUE; // required for full GLEW functionality for OpenGL 3.0+
	GLenum err = glewInit();
	if (GLEW_OK != err) { // Problem: glewInit failed, something is seriously wrong.
		cerr << "Error: " << glewGetErrorString(err) << endl;
		abort(); // Unrecoverable error
	}



	// Print out our OpenGL verisions
	cout << "Using OpenGL " << glGetString(GL_VERSION) << endl;
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;
	cout << "Using GLFW " << glfwMajor << "." << glfwMinor << "." << glfwRevision << endl;



	// Attach input callbacks to g_window
	glfwSetCursorPosCallback(g_window, cursorPosCallback);
	glfwSetMouseButtonCallback(g_window, mouseButtonCallback);
	glfwSetScrollCallback(g_window, scrollCallback);
	glfwSetKeyCallback(g_window, keyCallback);
	glfwSetCharCallback(g_window, charCallback);



	// Enable GL_ARB_debug_output if available. Not nessesary, just helpful
	if (glfwExtensionSupported("GL_ARB_debug_output")) {
		// This allows the error location to be determined from a stacktrace
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		// Set the up callback
		glDebugMessageCallbackARB(debugCallbackARB, nullptr);
		glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
		cout << "GL_ARB_debug_output callback installed" << endl;
	}
	else {
		cout << "GL_ARB_debug_output not available. No worries." << endl;
	}


	// Initialize Geometry/Material/Lights
	// YOUR CODE GOES HERE
	// ...
	initLight();
	initShader();

	g_geometryMain = new GeometryMain();

	g_engine = new Engine();

	cout << "--WASD for camera movement." << endl;
	cout << "--Left click for camera rotation." << endl;
	cout << "--Space and left Ctrl for up / down." << endl;

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(g_window)) {

		// Make sure we draw to the WHOLE window
		int width, height;
		glfwGetFramebufferSize(g_window, &width, &height);

		// Main Render
		render(width, height);

		// Swap front and back buffers
		glfwSwapBuffers(g_window);

		// Poll for and process events
		glfwPollEvents();
	}

	glfwTerminate();
}






//-------------------------------------------------------------
// Fancy debug stuff
//-------------------------------------------------------------

// function to translate source to string
string getStringForSource(GLenum source) {

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		return("API");
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		return("Window System");
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		return("Shader Compiler");
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		return("Third Party");
	case GL_DEBUG_SOURCE_APPLICATION:
		return("Application");
	case GL_DEBUG_SOURCE_OTHER:
		return("Other");
	default:
		return("n/a");
	}
}

// function to translate severity to string
string getStringForSeverity(GLenum severity) {

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		return("HIGH!");
	case GL_DEBUG_SEVERITY_MEDIUM:
		return("Medium");
	case GL_DEBUG_SEVERITY_LOW:
		return("Low");
	default:
		return("n/a");
	}
}

// function to translate type to string
string getStringForType(GLenum type) {
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		return("Error");
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		return("Deprecated Behaviour");
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		return("Undefined Behaviour");
	case GL_DEBUG_TYPE_PORTABILITY:
		return("Portability Issue");
	case GL_DEBUG_TYPE_PERFORMANCE:
		return("Performance Issue");
	case GL_DEBUG_TYPE_OTHER:
		return("Other");
	default:
		return("n/a");
	}
}

// actually define the function
void APIENTRY debugCallbackARB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar* message, GLvoid*) {
	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) return;

	cerr << endl; // extra space

	cerr << "Type: " <<
		getStringForType(type) << "; Source: " <<
		getStringForSource(source) << "; ID: " << id << "; Severity: " <<
		getStringForSeverity(severity) << endl;

	cerr << message << endl;

	if (type == GL_DEBUG_TYPE_ERROR_ARB) throw runtime_error("");
}