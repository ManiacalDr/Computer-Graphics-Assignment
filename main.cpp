// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

// Include GLEW. Always include it before gl.h and glfw3.h, since it's a bit magic.
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>
// Include GLM
#include <glm/ext.hpp>
using namespace glm;

#include <shader.hpp>

#include<GL/glu.h>
#define reportError(s) _ReportError(__LINE__, (s))

// Note that it uses gluErrorString, so you must inlcude glu32.lib in your visual studio projects and -lGLU on linux
void _ReportError(int ln, const std::string str) {
	GLuint err = glGetError();
	if (!err) return;
	const GLubyte* glerr = gluErrorString(err);
	printf("******************************************\n%d: %s: GLError %d: %s\n", ln, str.c_str(), err, glerr);
}

struct Material {
	vec3 diffuse;
	vec3 specular;
	GLfloat emissivity;
};

const vec3 ambiantlight = vec3(0.2, 0.2, 0.2);
const int arraysize = 1638;
int place = 0;
int object = 1;
int renderState = 0;
int rotateState = 0;
int lightingState = 0;

const int numVBO = 1;
const int numVertices = 3;

GLuint VBO;
GLuint VAO;

GLuint programID;
GLuint programID2;

GLuint model;
GLuint view;
GLuint proj;

GLuint mode;
GLuint lightPos;
GLuint emissivity;
GLuint ambiant;
GLuint diffuse;
GLuint spec;
GLuint normal;
GLuint cameraloc;

GLuint model2;
GLuint view2;
GLuint proj2;

GLuint mode2;
GLuint lightPos2;
GLuint emissivity2;
GLuint ambiant2;
GLuint diffuse2;
GLuint spec2;
GLuint normal2;
GLuint cameraloc2;

int VBOPosition[20];

static const Material materials[] = {  
	// if we want to change the material of an object, instead of changeing all of their numbers, change the order of these
	// however this will change everything sharing that position.
	{{0.55,0.55,0.55},{0.7,0.7,0.7},32},// White Plastic
	{ {0.4,0.4,0.4},{0.774597,0.774597,0.774597},0.6 },//Chrome
	{ {0.18275,0.17,0.22525},{0.332741,0.328634,0.346435},38.4 },//Obsidion
	{ {0.7038,0.27048,0.0828},{0.256777,0.137622,0.086014},12.8 },//Copper
};

static const vec3 tetrahedronGeometry[] = {
	{1.0f, 1.0f, 1.0f,}, //1
	{1.0f, -1.0f, -1.0f,},//2
	{-1.0f, 1.0f, -1.0f,}, //3
	{-1.0f, -1.0f, 1.0f,}, //4
};

static const vec3 cubeGeometry[] = {
	{1.0f, 1.0f, 1.0f,}, //1
	{-1.0f, 1.0f, 1.0f,},
	{1.0f, -1.0f, 1.0f,},
	{1.0f, 1.0f, -1.0f,},
	{-1.0f, -1.0f, 1.0f,}, //4
	{-1.0f, 1.0f, -1.0f,}, //3
	{1.0f, -1.0f, -1.0f,},//2
	{-1.0f, -1.0f, -1.0f,},//Numbered ones make up the Tetrahedron
};

static const vec3 octahedronGeometry[] = {
	{1.0f, 0.0f, 0.0f,},
	{-1.0f, 0.0f, 0.0f,},
	{0.0f, 1.0f, 0.0f,},
	{0.0f, -1.0f, 0.0f,},
	{0.0f, 0.0f, 1.0f,},
	{0.0f, 0.0f, -1.0f,},
};

static const vec3 dodecahedronGeometry[] = {
	{1.0f, 1.0f, 1.0f,},
	{-1.0f, 1.0f, 1.0f,},
	{1.0f, -1.0f, 1.0f,},
	{1.0f, 1.0f, -1.0f,},
	{-1.0f, -1.0f, 1.0f,},
	{-1.0f, 1.0f, -1.0f,},
	{1.0f, -1.0f, -1.0f,},
	{-1.0f, -1.0f, -1.0f,},//7
	{0.0f, 1.0f / 1.6180f, 1.6180f,},
	{0.0f, -1.0f / 1.6180f, 1.6180f,},
	{0.0f, 1.0f / 1.6180f, -1.6180f,},
	{0.0f, -1.0f / 1.6180f, -1.6180f,},//11
	{1.0f / 1.6180f, 1.6180f, 0.0f,},
	{-1.0f / 1.6180f, 1.6180f, 0.0f,},
	{1.0f / 1.6180f, -1.6180f, 0.0f,},
	{-1.0f / 1.6180f, -1.6180f, 0.0f,},//15
	{1.6180f, 0.0f, 1.0f / 1.6180f,},
	{-1.6180f, 0.0f, 1.0f / 1.6180f,},
	{1.6180f, 0.0f, -1.0f / 1.6180f,},
	{-1.6180f, 0.0f, -1.0f / 1.6180f,},//19
};

const GLfloat X = 1.0f / 1.6180f;
const GLfloat Z = 1.0f;
const GLfloat N = 0.f;

static const vec3 icosahedronGeometry[] = {
	{-X,N,Z}, {X,N,Z}, {-X,N,-Z}, {X,N,-Z},
  {N,Z,X}, {N,Z,-X}, {N,-Z,X}, {N,-Z,-X},
  {Z,X,N}, {-Z,X, N}, {Z,-X,N}, {-Z,-X, N}
};

static const vec3 houseGeometry[]{
	{-8, -5, 0},
	{-5, -5, 0},
	{-5, -5, 4.5},
	{0, -5, 4.5},
	{0, -5, 8},
	{8, -5, 8},
	{8, -5, -8},
	{-8, -5, -8},//Box Bottom 7

	{-8, 0, 0},
	{-5, 0, 0},
	{-5, 0, 4.5},
	{0, 0, 4.5},
	{0, 0, 8},
	{8, 0, 8},
	{8, 0, -8},
	{-8, 0, -8},//Box Top 15

	{4, 2.5, 5},
	{4, 2.5, 2.5},
	{1.5, 4, .5},
	{1.5, 4, -2.5},
	{0, 5, -3.5},
	{0, 5, -5},//Roof Top 21

	{-4, -4, 4.5},
	{-1, -4, 4.5},
	{-1, -1, 4.5},
	{-4, -1, 4.5},//Front Window 25

	{8, -2.5, 1.5},
	{8, -2.5, -1.5},
	{8, 0, -1.5},
	{8, 0, 1.5},//Right Window 29

	{5.5, -2.5, -8},
	{3.5, -2.5, -8},
	{3.5, 0, -8},
	{5.5, 0, -8},//Back Left Window 33

	{-4.5, -3.5, -8},
	{-6.5, -3.5, -8},
	{-6.5, -0.5, -8},
	{-4.5, -0.5, -8},//Back Right Window 37

	{-8, -2.5, -6.5},
	{-8, -2.5, -5},
	{-8, 0, -5},
	{-8, 0, -6.5},//Left Left Window 41

	{-8, -2.5, -3},
	{-8, -2.5, -1.5},
	{-8, 0, -1.5},
	{-8, 0, -3},//Left Right Window 45

	{1, -5, 8},
	{7, -5, 8},
	{7, -1, 8},
	{1, -1, 8},//Garage Door 49

	{-7.5, -5, 0},
	{-5.5, -5, 0},
	{-5.5, -1, 0},
	{-7.5, -1, 0},//Front Door 53

	{-1.5, -5, -8},
	{-3.5, -5, -8},
	{-3.5, -1.5, -8},
	{-1.5, -1.5, -8},//Back Door 57

	{-2.5, 0, -8},
	{-4, 0, -8},
	{-8, -5, -5},
	{-8, -5, -3},
};

static const GLint octahedronTopology[][3] = {
	{2, 4, 0},
	{2, 1, 4},
	{2, 5, 1},
	{2, 0, 5},
	{3, 4, 0},
	{3, 1, 4},
	{3, 5, 1},
	{3, 0, 5},
};

static const GLint dodecahedronTopology[][5] = {
	{1, 8, 0, 12, 13,},
	{3, 10, 5, 13, 12,},
	{1, 13, 5, 19, 17,},
	{3, 12, 0, 16, 18,},
	{1, 17, 4, 9, 8,},
	{2, 16, 0, 8, 9,},
	{7, 19, 5, 10, 11,},
	{3, 16, 6, 11, 10,},
	{7, 15, 4, 17, 19,},
	{6, 14, 2, 16, 18},
	{2, 9, 4, 15, 14},
	{7, 11, 6, 14, 15,},
};

static const GLint icosahedronTopology[][3] = {
	{0,4,1},{0,9,4},{9,5,4},{4,5,8},{4,8,1},
	{8,10,1},{8,3,10},{5,3,8},{5,2,3},{2,7,3},
	{7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
	{6,1,10},{9,0,11},{9,11,2},{9,2,5},{7,2,11},
};

static const GLint houseTopology[][4]{ //This defines the walls, does not define features such as doors and windows
	{7,6,0,1},
	{1,6,2,3},
	{3,6,4,5},

	{0,8,50,53},
	{8,9,53,52},
	{51,52,1,9},
	{1,9,2,10},

	{10,11,25,24},
	{11,3,24,23},
	{2,22,3,23},
	{2,10,22,25},
	{3,11,4,12},

	{4,12,46,49},
	{12,13,49,48},
	{48,13,47,5},

	{5,13,26,29},
	{5,26,6,27},
	{28,14,27,6},

	{14,33,6,30},
	{6,30,54,31},
	{32,57,31,54},
	{32,58,57,56},
	{58,59,56,37},
	{56,37,55,34},
	{34,35,55,7},
	{36,15,35,7},
	{59,15,37,36},

	{15,41,7,38},
	{38,39,7,60},
	{40,45,39,42},
	{39,42,60,61},
	{42,43,61,0},
	{44,8,43,0},

	{14,17,13,16},
	{14,21,19,20},
	{14,19,17,18},
	{8,20,15,21},
	{10,18,9,19},
	{9,19,8,20},
	{17,18,11,10},
	{12,16,11,17},//Roof
};

static const GLint roofTopology[][3]{
	{13,16,12},
	{15,21,14},
};

static const vec3 Colors[] = {
	{0.0, 0.0, 0.0},  //black
	{1.0, 0.0, 0.0},  // red
	{1.0, 1.0, 0.0},  // yellow
	{0.0, 1.0, 0.0},  // green
	{0.0, 0.0, 1.0},  // blue
	{1.0, 0.0, 1.0},  // magenta
	{1.0, 1.0, 1.0},  // white
	{0.0, 1.0, 1.0},   // cyan
};

static GLfloat vertexPositions[arraysize];
static GLfloat vertexNormals[arraysize];

vec3 calcNormal(vec3 point1, vec3 point2, vec3 point3) {
	vec3 vector1 = point2 - point1;
	vec3 vector2 = point3 - point1;

	vec3 ans = cross(vector1, vector2);

	//printf("Normal :%f, %f, %f\n", ans.x, ans.y, ans.z);
	return ans;
};

void triangle(int a, int b, int c,const vec3 (*ptr)) {
	static int index = 0;
	vec3 temp = calcNormal(ptr[a], ptr[b], ptr[c]);

	vertexPositions[index++] = ptr[a].x; 
	vertexPositions[index++] = ptr[a].y;
	vertexPositions[index++] = ptr[a].z;

	vertexPositions[index++] = ptr[b].x;
	vertexPositions[index++] = ptr[b].y;
	vertexPositions[index++] = ptr[b].z;

	vertexPositions[index++] = ptr[c].x;
	vertexPositions[index++] = ptr[c].y;
	vertexPositions[index++] = ptr[c].z;

	for (int i = index - 9; i < index; i+=3) {
		int j = i + 1;
		int k = i + 2;

		vertexNormals[i] = temp.x;
		vertexNormals[j] = temp.y;
		vertexNormals[k] = temp.z;
	}
	
	//printf("Points :%f, %f, %f\n", ptr[a].x, ptr[a].y, ptr[a].z);
	//printf("Points :%f, %f, %f\n", ptr[b].x, ptr[b].y, ptr[b].z);
	//printf("Points :%f, %f, %f\n\n", ptr[c].x, ptr[c].y, ptr[c].z);

	place += 1;

	reportError("tri");
};

void tetrahedron(int a, int b, int c, int d) {
	triangle(a, b, c, &cubeGeometry[0]);
	triangle(a, c, d, &cubeGeometry[0]);
	triangle(a, d, b, &cubeGeometry[0]);
	triangle(d, c, b, &cubeGeometry[0]);
};

void quad(int a, int b, int c, int d,const vec3 (*ptr)) {
	triangle(a, c, b, ptr);
	triangle(a, d, c, ptr);

	reportError("quad");
};

void colorcube(int a, int b, int c, int d, int e, int f, int g, int h) { 
	quad(b, a, d, c, &cubeGeometry[0]);
	quad(c, d, h, g, &cubeGeometry[0]);
	quad(d, a, e, h, &cubeGeometry[0]);
	quad(g, f, b, c, &cubeGeometry[0]);
	quad(e, f, g, h, &cubeGeometry[0]);
	quad(f, e, a, b, &cubeGeometry[0]);

	reportError("cube");
};

 void FillBuffer() {
	//The purpose of this is to take indexed topology and turn it into a 
	//list that can be used directly by the VBO without an IBO
	 int VBOindex = 0;

	 VBOPosition[VBOindex++] = place;
	 tetrahedron(0, 6, 5, 4);

	 VBOPosition[VBOindex++] = place;
	 colorcube(4,1,0,2,7,5,3,6);

	 VBOPosition[VBOindex++] = place;
	 for (int i = 0; i < 8; i++) {
		 triangle(octahedronTopology[i][0], octahedronTopology[i][1], octahedronTopology[i][2], &octahedronGeometry[0]);
	 }

	 VBOPosition[VBOindex++] = place;
	 for (int i = 0; i < 12; i++) {
		 triangle(dodecahedronTopology[i][0], dodecahedronTopology[i][1], dodecahedronTopology[i][2], &dodecahedronGeometry[0]);
		 triangle(dodecahedronTopology[i][0], dodecahedronTopology[i][2], dodecahedronTopology[i][3], &dodecahedronGeometry[0]);
		 triangle(dodecahedronTopology[i][0], dodecahedronTopology[i][3], dodecahedronTopology[i][4], &dodecahedronGeometry[0]);
	 }

	 VBOPosition[VBOindex++] = place;
	 for (int i = 0; i < 20; i++) {
		 triangle(icosahedronTopology[i][0], icosahedronTopology[i][1], icosahedronTopology[i][2], &icosahedronGeometry[0]);
	 }

	 VBOPosition[VBOindex++] = place;
	 for (int i = 0; i < 33; i++) {//walls
		 int j = 0;
		 quad(houseTopology[i][0], houseTopology[i][1], houseTopology[i][3], houseTopology[i][2], &houseGeometry[0]);
	 }

	 VBOPosition[VBOindex++] = place;
	 for (int i = 33; i < 41; i++) {//roof
		 int j = 0;
		 quad(houseTopology[i][0], houseTopology[i][1], houseTopology[i][3], houseTopology[i][2], &houseGeometry[0]);
	 }

	 for (int i = 0; i < 2; i++) {
		 triangle(roofTopology[i][0], roofTopology[i][1], roofTopology[i][2], &houseGeometry[0]);
	 }

	 VBOPosition[VBOindex++] = place;
	 for (int i = 22; i < 45; i += 4) {//windows
		 int j = i + 1;
		 int k = i + 2;
		 int l = i + 3;
		 quad(i, j, k, l, &houseGeometry[0]);
	 }

	 VBOPosition[VBOindex++] = place;
	
	quad(46, 47, 48, 49, &houseGeometry[0]);//garage door

	 VBOPosition[VBOindex++] = place;
	 for (int i = 50; i < 57; i += 4) {
		 int j = i + 1;
		 int k = i + 2;
		 int l = i + 3;
		 quad(i, j, k, l, &houseGeometry[0]);//other doors
	 }

	 VBOPosition[VBOindex++] = place;

	 /*for (int i = 0; i < arraysize; i++)
		 printf("Points :%f, %f, %f\n", vertexPositions[i].x, vertexPositions[i].y, vertexPositions[i].z);*/

	 //for(int i = 0; i < 7; i++)
		// std::cout << VBOPosition[i] << "\n";

	//std::cout << index;
	reportError("fill");
};

void initialize() {

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	glGenBuffers(numVBO, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions) + sizeof(vertexNormals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexPositions), vertexPositions);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertexPositions), sizeof(vertexNormals), vertexNormals);

	programID = LoadShaders("GouraudVertexShader.txt", "GouraudFragmentShader.txt");

	GLint vPosition = glGetAttribLocation(programID, "vPosition");
	if (vPosition < 0) std::cerr << "couldn't find vPosition in shader\n";
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

	GLint vNormal = glGetAttribLocation(programID, "vNormal");
	if (vNormal < 0) std::cerr << "couldn't find vNormal in shader\n";
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

	/*Start definition of unfiform variables*/

	model = glGetUniformLocation(programID, "model");
	if (model < 0) std::cerr << "couldn't find model in shader\n";

	view = glGetUniformLocation(programID, "view");
	if (view < 0) std::cerr << "couldn't find view in shader\n";

	proj = glGetUniformLocation(programID, "proj");
	if (proj < 0) std::cerr << "couldn't find proj in shader\n";

	mode = glGetUniformLocation(programID, "mode");
	if (mode < 0) std::cerr << "couldn't find mode in shader\n";

	lightPos = glGetUniformLocation(programID, "lightPosition");
	if (lightPos < 0) std::cerr << "couldn't find lightPosition in shader\n";

	emissivity = glGetUniformLocation(programID, "emissivity");
	if (emissivity < 0) std::cerr << "couldn't find emissivity in shader\n";

	ambiant = glGetUniformLocation(programID, "ambiant");
	if (ambiant < 0) std::cerr << "couldn't find ambiant in shader\n";

	diffuse = glGetUniformLocation(programID, "diffuse");
	if (diffuse < 0) std::cerr << "couldn't find diffuse in shader\n";

	spec = glGetUniformLocation(programID, "spec");
	if (spec < 0) std::cerr << "couldn't find ambiant in shader\n";

	normal = glGetUniformLocation(programID, "normalMat");
	if (normal < 0) std::cerr << "couldn't find normalMat in shader\n";

	cameraloc = glGetUniformLocation(programID, "cameraloc");
	if (cameraloc < 0) std::cerr << "couldn't find cameraloc in shader\n";

	programID2 = LoadShaders("PhongVertexShader.txt", "PhongFragmentShader.txt");

	vPosition = glGetAttribLocation(programID2, "vPosition");
	if (vPosition < 0) std::cerr << "couldn't find vPosition in shader\n";
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

	vNormal = glGetAttribLocation(programID2, "vNormal");
	if (vNormal < 0) std::cerr << "couldn't find vNormal in shader\n";
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

	/*Start definition of unfiform variables*/

	model2 = glGetUniformLocation(programID2, "model");
	if (model < 0) std::cerr << "couldn't find model in shader\n";

	view2 = glGetUniformLocation(programID2, "view");
	if (view < 0) std::cerr << "couldn't find view in shader\n";

	proj2 = glGetUniformLocation(programID2, "proj");
	if (proj < 0) std::cerr << "couldn't find proj in shader\n";

	mode2 = glGetUniformLocation(programID2, "mode");
	if (mode < 0) std::cerr << "couldn't find mode in shader\n";

	lightPos2 = glGetUniformLocation(programID2, "lightPosition");
	if (lightPos < 0) std::cerr << "couldn't find lightPosition in shader\n";

	emissivity2 = glGetUniformLocation(programID2, "emissivity");
	if (emissivity < 0) std::cerr << "couldn't find emissivity in shader\n";

	ambiant2 = glGetUniformLocation(programID2, "ambiant");
	if (ambiant < 0) std::cerr << "couldn't find ambiant in shader\n";

	diffuse2 = glGetUniformLocation(programID2, "diffuse");
	if (diffuse < 0) std::cerr << "couldn't find diffuse in shader\n";

	spec2 = glGetUniformLocation(programID2, "spec");
	if (spec < 0) std::cerr << "couldn't find ambiant in shader\n";

	normal2 = glGetUniformLocation(programID2, "normalmat");
	if (normal < 0) std::cerr << "couldn't find normalMat in shader\n";

	cameraloc2 = glGetUniformLocation(programID2, "cameraloc");
	if (cameraloc < 0) std::cerr << "couldn't find cameraloc in shader\n";

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.70, 1);
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	reportError("init");
}

void glfwErrorCB(int error, const char* description) {
	fputs(description, stderr);
}

void KeyboardCB(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		object = 1;
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		object = 2;
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		object = 3;
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		object = 4;
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
		object = 5;
	if (key == GLFW_KEY_6 && action == GLFW_PRESS)
		object = 6;
	if (key == GLFW_KEY_7 && action == GLFW_PRESS)
		object = 7;
	if (key == GLFW_KEY_8 && action == GLFW_PRESS)
		object = 8;
	if (key == GLFW_KEY_9 && action == GLFW_PRESS)
		object = 9;

	//if (key == GLFW_KEY_W && action == GLFW_PRESS)//move forward
		
	//if (key == GLFW_KEY_A && action == GLFW_PRESS)//move left
		
	//if (key == GLFW_KEY_S && action == GLFW_PRESS)//move back
		
	//if (key == GLFW_KEY_D && action == GLFW_PRESS)//move right
		

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		renderState = 1;
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
		renderState = 0;

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		lightingState = 1;
	if (key == GLFW_KEY_G && action == GLFW_PRESS)
		lightingState = 0;

	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		if (rotateState == 0)
			rotateState = 1;
		else if (rotateState == 1)
			rotateState = 0;
	}
}

void renderWorld() {
	static float theta = 0.0f;

	mat4 m = rotate(scale(mat4(1.0),vec3(.5)), theta, vec3(0.0,1.0,0.0));
	mat4 p = perspective(45.0f, 3.0f / 3.0f, .01f, 1000000.0f);
	vec3 pointOfInterest(0, 0, 0);
	vec3 cameraLocation(-5.0f, 5.0f, -5.0f);
	vec3 cameraUp(1.0f, 1.0f, 1.0f);

	mat4 v = lookAt(cameraLocation, pointOfInterest, cameraUp);

	mat4 n = transpose(inverse(m));

	vec3 a = ambiantlight;
	vec3 d = materials[0].diffuse;
	vec3 s = materials[0].specular;
	float e = materials[0].emissivity;
	vec3 lp[] = { vec3(10.0f, 10.0f, 10.0f), vec3(1, 0, -1), vec3(0,-1,0) };

	lp[0] =  vec3(vec4(lp[0],1.0) * rotate(mat4(1.0), theta, vec3(0.0, 1.0, 0.0)));

	if (renderState == 1)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (renderState == 0)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (lightingState == 0)
	{
		glUseProgram(programID);

		glUniformMatrix4fv(model, 1, GL_FALSE, value_ptr(m));
		glUniformMatrix4fv(view, 1, GL_FALSE, value_ptr(v));
		glUniformMatrix4fv(proj, 1, GL_FALSE, value_ptr(p));

		glUniformMatrix4fv(normal, 1, GL_FALSE, value_ptr(n));

		glUniform3fv(cameraloc, 1, value_ptr(cameraLocation));

		glUniform3fv(ambiant, 1, value_ptr(a));
		glUniform3fv(spec, 1, value_ptr(s));
		glUniform3fv(diffuse, 1, value_ptr(d));

		glUniform1f(emissivity, e);
		glUniform1i(mode, 1);

		glUniform3fv(lightPos, 3, value_ptr(lp[0]));
	}

	if (lightingState == 1)
	{
		glUseProgram(programID2);

		glUniformMatrix4fv(model2, 1, GL_FALSE, value_ptr(m));
		glUniformMatrix4fv(view2, 1, GL_FALSE, value_ptr(v));
		glUniformMatrix4fv(proj2, 1, GL_FALSE, value_ptr(p));

		glUniformMatrix4fv(normal2, 1, GL_FALSE, value_ptr(n));

		glUniform3fv(cameraloc2, 1, value_ptr(cameraLocation));

		glUniform3fv(ambiant2, 1, value_ptr(a));
		glUniform3fv(spec2, 1, value_ptr(s));
		glUniform3fv(diffuse2, 1, value_ptr(d));

		glUniform1f(emissivity2, e);
		glUniform1i(mode2, 1);

		glUniform3fv(lightPos2, 3, value_ptr(lp[0]));
	}

	switch (object){
		case 1:
		{
			for (int i = VBOPosition[1]; i < VBOPosition[2]; i++) {
				glDrawArrays(GL_TRIANGLES, i * 3, numVertices);
			}
		}
		break;
		case 2:
		{
			for (int i = VBOPosition[0]; i < VBOPosition[1]; i++) {
				glDrawArrays(GL_TRIANGLES, i * 3, numVertices);
			}
		}
		break;
		case 3:
		{
			for (int i = VBOPosition[2]; i < VBOPosition[3]; i++) {
				glDrawArrays(GL_TRIANGLES, i * 3, numVertices);
			}
		}
		break;
		case 4:
		{
			for (int i = VBOPosition[3]; i < VBOPosition[4]; i++) {
				glDrawArrays(GL_TRIANGLES, i * 3, numVertices);
			}
		}
		break;
		case 5:
		{
			for (int i = VBOPosition[4]; i < VBOPosition[5]; i++) {
				glDrawArrays(GL_TRIANGLES, i * 3, numVertices);
			}
		}
		break;
		case 6:
		{
			m = scale(mat4(1.0), vec3(.325));
			if (lightingState == 0)
			{
				glUniformMatrix4fv(model, 1, GL_FALSE, value_ptr(m));
			}

			if (lightingState == 1)
			{
				glUniformMatrix4fv(model2, 1, GL_FALSE, value_ptr(m));
			}

			for (int i = VBOPosition[5]; i < VBOPosition[6]; i++) {//Walls
				glDrawArrays(GL_TRIANGLES, i * 3, 3);
			}

			glUniform3fv(spec, 1, value_ptr(materials[2].specular));
			glUniform3fv(diffuse, 1, value_ptr(materials[2].diffuse));

			glUniform1f(emissivity, materials[2].emissivity);

			for (int i = VBOPosition[6]; i < VBOPosition[7]; i++) {//Roof
				glDrawArrays(GL_TRIANGLES, i * 3, 3);
			}

			glUniform3fv(spec, 1, value_ptr(materials[1].specular));
			glUniform3fv(diffuse, 1, value_ptr(materials[1].diffuse));

			glUniform1f(emissivity, materials[1].emissivity);
			
			for (int i = VBOPosition[7]; i < VBOPosition[8]; i++) {//Windows
				glDrawArrays(GL_TRIANGLES, i * 3, 3);
			}

			for (int i = VBOPosition[8]; i < VBOPosition[9]; i++) {//Garage
				glDrawArrays(GL_TRIANGLES, i * 3, 3);
			}

			glUniform3fv(spec, 1, value_ptr(materials[3].specular));
			glUniform3fv(diffuse, 1, value_ptr(materials[3].diffuse));

			glUniform1f(emissivity, materials[3].emissivity);

			for (int i = VBOPosition[9]; i < VBOPosition[10]; i++) {//Doors
				glDrawArrays(GL_TRIANGLES, i * 3, 3);
			}
		}
		break;
		case 7:
		{

		}
		default: //can be used for debugging
			break;
	}

	if (rotateState == 0) theta += 0;
	if (rotateState == 1) theta += .0001;

	if (theta > 360) theta = 0;

	glFlush();
};

void clean() {
	// Cleanup VBO
	glDeleteBuffers(numVBO, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
};

int main() {
	// Initialise GLFW
	glewExperimental = true; // Needed for core profile
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	// Open a window and create its OpenGL context
	GLFWwindow* window;

	glfwSetErrorCallback(glfwErrorCB);

	window = glfwCreateWindow(1024, 768, "Lighting", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, KeyboardCB);//must be called after creating window

	glfwMakeContextCurrent(window); // Initialize GLEW
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	std::cout << "Running OpenGL Version " << glGetString(GL_VERSION)
		<< " using " << glGetString(GL_RENDERER) << "\n";

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	FillBuffer();

	//std::cout << vertexPositions.size() << " ";
	//std::cout << sizeof(vertexPositions)/ sizeof(vertexPositions[0]) << "\n";

	initialize();

	do {
		renderWorld();
		glfwSwapBuffers(window);
		glfwPollEvents();
	} // Check if the ESC key was pressed or the window was closed
	while (!glfwWindowShouldClose(window));
	clean();

	return 0;
}
