#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>
#include <thread>

#define M_PI 3.141592654f

unsigned int g_windowWidth = 800;
unsigned int g_windowHeight = 600;
char* g_windowName = "HW3-3D-Basics";

GLFWwindow* g_window;

// Model data
std::vector<float> g_meshVertices;
std::vector<float> g_meshNormals;
std::vector<unsigned int> g_meshIndices;
GLfloat g_modelViewMatrix[16];

// Default options
bool enablePersp = true;
bool teapotSpin = false;
bool enableDolly = false;
bool showCheckerboard = false;

// Dolly zoom options 
float fov = M_PI / 4.f;
float distance = 4.5f;

// Auxiliary math functions
float dotProduct(const float* a, const float* b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void crossProduct(const float* a, const float* b, float* r)
{
	r[0] = a[1] * b[2] - a[2] * b[1];
	r[1] = a[2] * b[0] - a[0] * b[2];
	r[2] = a[0] * b[1] - a[1] * b[0];
}

float radianToDegree(float angleInRadian) {
	return angleInRadian * 180.f / M_PI;
}

void normalize(float* a)
{
	const float len = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);

	a[0] /= len;
	a[1] /= len;
	a[2] /= len;
}

void computeNormals()
{
	g_meshNormals.resize(g_meshVertices.size());

	// TASK 1
	// The code below sets all normals to point in the z-axis, so we get a boring constant gray color
	// The following should be replaced with your code for normal computation
	
	for (int i = 0; i < g_meshIndices.size() / 3; i++) {

		//get indices on triangles
		int p1 = g_meshIndices[3 * i];
		int p2 = g_meshIndices[3 * i + 1];
		int p3 = g_meshIndices[3 * i + 2];

		//get vertices
		float x1 = g_meshVertices[3 * p1];
		float y1 = g_meshVertices[3 * p1 + 1];
		float z1 = g_meshVertices[3 * p1 + 2];
		float x2 = g_meshVertices[3 * p2];
		float y2 = g_meshVertices[3 * p2 + 1];
		float z2 = g_meshVertices[3 * p2 + 2];
		float x3 = g_meshVertices[3 * p3];
		float y3 = g_meshVertices[3 * p3 + 1];
		float z3 = g_meshVertices[3 * p3 + 2];
		
		float ux = x2 - x1;
		float uy = y2 - y1;
		float uz = z2 - z1;
		float vx = x3 - x1;
		float vy = y3 - y1;
		float vz = z3 - z1;
		
		//cross product
		float nx = (uy * vz) - (uz * vy);
		float ny = (uz * vx) - (ux * vz);
		float nz = (ux * vy) - (uy * vx);

		float norm = sqrt(nx * nx + ny * ny + nz * nz);

		//normalize
		nx /= norm;
		ny /= norm;
		nz /= norm;

		//add normals to g_meshNormals
		g_meshNormals[3 * p1] = nx;
		g_meshNormals[3 * p1 + 1] = ny;
		g_meshNormals[3 * p1 + 2] = nz;
		g_meshNormals[3 * p2] = nx;
		g_meshNormals[3 * p2 + 1] = ny;
		g_meshNormals[3 * p2 + 2] = nz;
		g_meshNormals[3 * p3] = nx;
		g_meshNormals[3 * p3 + 1] = ny;
		g_meshNormals[3 * p3 + 2] = nz;
		

		
	}

	for (int v = 0; v < g_meshNormals.size() / 3; ++v)
	{
		//normalize again
		normalize(&g_meshNormals[3 * v]);
		normalize(&g_meshNormals[3 * v+1]);
		normalize(&g_meshNormals[3 * v+2]);
	}
}

void loadObj(std::string p_path)
{
	std::ifstream nfile;
	nfile.open(p_path);
	std::string s;

	while (nfile >> s)
	{
		if (s.compare("v") == 0)
		{
			float x, y, z;
			nfile >> x >> y >> z;
			g_meshVertices.push_back(x);
			g_meshVertices.push_back(y);
			g_meshVertices.push_back(z);
		}		
		else if (s.compare("f") == 0)
		{
			std::string sa, sb, sc;
			unsigned int a, b, c;
			nfile >> sa >> sb >> sc;

			a = std::stoi(sa);
			b = std::stoi(sb);
			c = std::stoi(sc);

			g_meshIndices.push_back(a - 1);
			g_meshIndices.push_back(b - 1);
			g_meshIndices.push_back(c - 1);
		}
		else
		{
			std::getline(nfile, s);
		}
	}

	computeNormals();

	std::cout << p_path << " loaded. Vertices: " << g_meshVertices.size() / 3 << " Triangles: " << g_meshIndices.size() / 3 << std::endl;
}

double getTime()
{
	return glfwGetTime();
}

void glfwErrorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error " << error << ": " << description << std::endl;
	exit(1);
}

void togglePerspective() {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Perspective Projection
	if (enablePersp)
	{
		
		float halfwidth = distance * tan(fov / 2);
		double time = getTime();
		// Dolly zoom computation
		if (enableDolly) {
			// TASK 3
			// Your code for dolly zoom computation goes here
			// You can use getTime() to change fov over time
			// distance should be recalculated here using the Equation 2 in the description file

			if (distance < 40 && distance > 1) {
				
				fov = abs(sin(time));
				distance = abs(halfwidth / tan(fov / 2));

			}
			else  {
				fov = abs(-sin(time));
				distance = abs(halfwidth / tan(fov / 2));

			}

		}

		float fovInDegree = radianToDegree(fov);
		gluPerspective(fovInDegree, (GLfloat)g_windowWidth / (GLfloat)g_windowHeight, 1.0f, 40.f);
	}
	// Othogonal Projection
	else
	{
		// Scale down the object for a better view in orthographic projection
		glScalef(0.5, 0.5, 0.5);

		// TASK 3
		// Your code for orthogonal projection goes here
		// (Hint: you can use glOrtho() function in OpenGL)
		float ratio = g_windowWidth / g_windowHeight;
		glOrtho(-1.5, 1.5,-ratio, ratio, 1.0f, 40.0f);
	}
}


void glfwKeyCallback(GLFWwindow* p_window, int p_key, int p_scancode, int p_action, int p_mods)
{
	if (p_key == GLFW_KEY_ESCAPE && p_action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(g_window, GL_TRUE);
	}
	if (p_key == GLFW_KEY_P && p_action == GLFW_PRESS) {

		// Perspective Projection
		enablePersp = true;
		togglePerspective();
		std::cout << "Perspective activated\n";

	}
	if (p_key == GLFW_KEY_O && p_action == GLFW_PRESS) {

		// Orthographic Projection
		enablePersp = false;
		togglePerspective();
		std::cout << "Orthogonal activated\n";

	}
	if (p_key == GLFW_KEY_S && p_action == GLFW_PRESS) {

		// Toggle Spinning
		if (!teapotSpin) {
			std::cout << "Teapot spinning on\n";
		}
		else {
			std::cout << "Teapot spinning off\n";
		}
		teapotSpin = !teapotSpin;
	}
	if (p_key == GLFW_KEY_D && p_action == GLFW_PRESS) {

		// Toggle dolly zoom
		if (!enableDolly)
		{
			std::cout << "Dolly zoom on\n";
		}
		else {
			std::cout << "Dolly zoom off\n";
		}
		enableDolly = !enableDolly;
	}
	if (p_key == GLFW_KEY_C && p_action == GLFW_PRESS) {

		// Show/hide Checkerboard
		if (!showCheckerboard)
		{
			std::cout << "Show checkerboard\n";
		}
		else {
			std::cout << "Hide checkerboard\n";
		}
		showCheckerboard = !showCheckerboard;
	}
}

void initWindow()
{
	// initialize GLFW
	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit())
	{
		std::cerr << "GLFW Error: Could not initialize GLFW library" << std::endl;
		exit(1);
	}

	g_window = glfwCreateWindow(g_windowWidth, g_windowHeight, g_windowName, NULL, NULL);
	if (!g_window)
	{
		glfwTerminate();
		std::cerr << "GLFW Error: Could not initialize window" << std::endl;
		exit(1);
	}

	// callbacks
	glfwSetKeyCallback(g_window, glfwKeyCallback);

	// Make the window's context current
	glfwMakeContextCurrent(g_window);

	// turn on VSYNC
	glfwSwapInterval(1);
}

void initGL()
{
	glClearColor(1.f, 1.f, 1.f, 1.0f);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
}

void printHotKeys() {
	std::cout << "\nHot Keys..\n"
		<< "Orthogonal Projection:  O\n"
		<< "Perspective Projection: P\n"
		<< "Toggle Spinning:        S\n"
		<< "Toggle Dolly Zoom:      D\n"
		<< "Show/hide Checkerboard: C\n"
		<< "Exit:                   Esc\n\n";
}

void clearModelViewMatrix()
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			g_modelViewMatrix[4 * i + j] = 0.0f;
		}
	}
}

void updateModelViewMatrix()
{	
	clearModelViewMatrix();

	// TASK 2
	// The following code sets a static modelView matrix
	// This should be replaced with code implementing Task 2
	// You can use getTime() to change rotation over time
	if (teapotSpin) {
		double time = getTime();
		g_modelViewMatrix[0] = cos(time);
		g_modelViewMatrix[2] = -sin(time);
		g_modelViewMatrix[5] = 1.0f;
		g_modelViewMatrix[8] = sin(time);
		g_modelViewMatrix[10] = cos(time);
		g_modelViewMatrix[14] = -distance;
		g_modelViewMatrix[15] = 1.0f;
	}else{

	g_modelViewMatrix[0] = 1.0f;
	g_modelViewMatrix[5] = 1.0f;
	g_modelViewMatrix[10] = 1.0f;
	g_modelViewMatrix[14] = -distance;
	g_modelViewMatrix[15] = 1.0f;
	}

}

void setModelViewMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	updateModelViewMatrix();
	glLoadMatrixf(g_modelViewMatrix);
}

void drawTeapot() {
	glBegin(GL_TRIANGLES);
	for (size_t f = 0; f < g_meshIndices.size(); ++f)
	{
		const float scale = 0.1f;
		const unsigned int idx = g_meshIndices[f];
		const float x = scale * g_meshVertices[3 * idx + 0];
		const float y = scale * g_meshVertices[3 * idx + 1];
		const float z = scale * g_meshVertices[3 * idx + 2];

		const float nx = g_meshNormals[3 * idx + 0];
		const float ny = g_meshNormals[3 * idx + 1];
		const float nz = g_meshNormals[3 * idx + 2];

		glNormal3f(nx, ny, nz);
		glVertex3f(x, y, z);
	}
	glEnd();
}
void renderTeapot() {
	drawTeapot();
}

void drawCheckerBoard() {
	int checkerCount = g_windowWidth;
	int checkerSize = (g_windowWidth) / checkerCount;

	glBegin(GL_QUADS);
	for (int i = 0; i < checkerCount; ++i) {
		for (int j = 0; j < checkerCount; ++j) {
			if ((i + j) % 2 == 0)
				glColor3f(0.0, 0.1, 1.0);
			else
				glColor3f(1.0, 0.0, 1.0);

			float x = i - checkerCount / 2; // to be between -1 and 1
			float z = j - checkerCount / 2;
			x *= checkerSize;
			z *= checkerSize;
			float y = -1.0f;
			glVertex3f(x, y, z);
			glVertex3f(x, y, z - checkerSize);
			glVertex3f(x + checkerSize, y, z - checkerSize);
			glVertex3f(x + checkerSize, y, z);
		}
	}
	glEnd();
}
void renderCheckerBoard() {

	/*
	/* If you want to keep checkerboard still while rotating the
	/* the teapot, you need to change the transformation for the
	/* checkerboard plane
	*/
	glMatrixMode(GL_MODELVIEW);
	clearModelViewMatrix();

	g_modelViewMatrix[0] = 1;
	g_modelViewMatrix[2] = 0;
	g_modelViewMatrix[5] = 1;
	g_modelViewMatrix[8] = 0;
	g_modelViewMatrix[10] = 1;
	g_modelViewMatrix[14] = -distance;
	g_modelViewMatrix[15] = 1.0f;
	
	glLoadMatrixf(g_modelViewMatrix);

	// Disable shading for the checkerboard
	glDisable(GL_LIGHTING);
	drawCheckerBoard();
	glEnable(GL_LIGHTING);
}

void render()
{
	togglePerspective();
	setModelViewMatrix();
	renderTeapot();
	if (showCheckerboard)
		renderCheckerBoard();
}

void renderLoop()
{
	while (!glfwWindowShouldClose(g_window))
	{
		// clear buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render();

		// Swap front and back buffers
		glfwSwapBuffers(g_window);

		// Poll for and process events
		glfwPollEvents();
	}
}

int main()
{
	initWindow();
	initGL();
	loadObj("data/teapot.obj");
	printHotKeys();
	renderLoop();
}
