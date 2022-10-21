#include <iostream>
using namespace std;


//--- OpenGL ---
#include "GL\glew.h"
#include "GL\wglew.h"
#pragma comment(lib, "glew32.lib")
//--------------

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"

#include "GL\freeglut.h"

#include "Images\FreeImage.h"


//objects
#include "Object/Object.h"

Object boxTest = Object();

///END MODEL LOADING

glm::mat4 ProjectionMatrix; // matrix for the orthographic projection

//	User Input
int	mouse_x=0, mouse_y=0;
bool LeftPressed = false;
int screenWidth=600, screenHeight=600;
float rotationSpeed = 0.003;
float speed = 0.01;

//	Rotation input
bool Left = false;
bool Right = false;
bool Up = false;
bool Down = false;
bool Home = false;
bool End = false;

//	Translation input
bool W = false;
bool A = false;
bool S = false;
bool D = false;
bool Q = false;
bool E = false;

//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();         //called in winmain to process keyboard input
void idle();		//idle function

/*************    START OF OPENGL FUNCTIONS   ****************/
void display()									
{
	//OpenGL Stuff
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewingMatrix = glm::mat4(1.0f);

	//use of glm::lookAt for viewing instead.
	viewingMatrix = glm::lookAt(glm::vec3(0,0,50), glm::vec3(0,0,-50), glm::vec3(0.0f, 1.0f, 0.0));
	
	//Object rendering
	boxTest.render(viewingMatrix, ProjectionMatrix);

	//OpenGL Stuff
	glFlush();
	glutSwapBuffers();
}

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth=width; screenHeight = height;           // to ensure the mouse coordinates match 
														// we will use these values to set the coordinate system
	glViewport(0,0,width,height);						// Reset The Current Viewport

	//Set the projection matrix
	ProjectionMatrix = glm::perspective(glm::radians(60.0f), (GLfloat)screenWidth/(GLfloat)screenHeight, 1.0f, 200.0f);
}
void init()
{
	//Open GL stuff
	glClearColor(1.0,1.0,1.0,0.0);						//sets the clear colour to yellow
														//glClear(GL_COLOR_BUFFER_BIT) in the display function
														//will clear the buffer to this colour
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//Object setup
	boxTest.setupShader("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag");
	boxTest.init("TestModels/axes.obj");
}

void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Left = true;
		break;
	case GLUT_KEY_RIGHT:
		Right = true;
		break;
	case GLUT_KEY_UP:
		Up = true;
		break;
	case GLUT_KEY_DOWN:
		Down = true;
		break;
	case GLUT_KEY_HOME:
		Home = true;
		break;
	case GLUT_KEY_END:
		End = true;
		break;
	}
}
void specialUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		Left = false;
		break;
	case GLUT_KEY_RIGHT:
		Right = false;
		break;
	case GLUT_KEY_UP:
		Up = false;
		break;
	case GLUT_KEY_DOWN:
		Down = false;
		break;
	case GLUT_KEY_HOME:
		Home = false;
		break;
	case GLUT_KEY_END:
		End = false;
		break;		
	}
}

void KeyDown(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'w':
			W = true;
			break;
		case 'a':
			A = true;
			break;
		case 's':
			S = true;
			break;
		case 'd':
			D = true;
			break;
		case 'q':
			Q = true;
			break;
		case 'e':
			E = true;
			break;
	}
}
void KeyUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		W = false;
		break;
	case 'a':
		A = false;
		break;
	case 's':
		S = false;
		break;
	case 'd':
		D = false;
		break;
	case 'q':
		Q = false;
		break;
	case 'e':
		E = false;
		break;
	}
}

void processKeys()
{
	//Rotation input
	float spinXinc = 0.0f, spinYinc = 0.0f, spinZinc = 0.0f;
	if (Left)
	{
		spinYinc = -rotationSpeed;
	}
	if (Right)
	{
		spinYinc = rotationSpeed;
	}
	if (Up)
	{
		spinXinc = rotationSpeed;
	}
	if (Down)
	{
		spinXinc = -rotationSpeed;
	}
	if (Home)
	{
		spinZinc = rotationSpeed;
	}
	if (End)
	{
		spinZinc = -rotationSpeed;
	}

	//Translation input
	float moveXinc = 0.0f, moveYinc = 0.0f, moveZinc = 0.0f;
	if (W)
	{
		moveYinc = speed;
	}
	if (S)
	{
		moveYinc = -speed;
	}
	if (D)
	{
		moveXinc = speed;
	}
	if (A)
	{
		moveXinc = -speed;
	}
	if (Q)
	{
		moveZinc = speed;
	}
	if (E)
	{
		moveZinc = -speed;
	}
	boxTest.UpdateTransform(glm::vec3(moveXinc, moveYinc, moveZinc), glm::vec3(spinXinc, spinYinc, spinZinc));
}

void idle()
{
	processKeys();
	glutPostRedisplay();
}
/**************** END OPENGL FUNCTIONS *************************/

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Space Simulation");

	//This initialises glew - it must be called after the window is created.
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << " GLEW ERROR" << endl;
	}

	//Check the OpenGL version being used
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
	cout << OpenGLVersion[0] << " " << OpenGLVersion[1] << endl;


	//initialise the objects for rendering
	init();

	glutReshapeFunc(reshape);
	//specify which function will be called to refresh the screen.
	glutDisplayFunc(display);

	glutSpecialFunc(special);
	glutSpecialUpFunc(specialUp);
	glutKeyboardFunc(KeyDown);
	glutKeyboardUpFunc(KeyUp);

	glutIdleFunc(idle);

	//starts the main loop. Program loops and calls callback functions as appropriate.
	glutMainLoop();

	return 0;
}
