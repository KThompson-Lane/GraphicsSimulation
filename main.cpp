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
#include "Object/Player.h"
#include "Object/Planet.h"

Player rocketShip = Player();
vector<Planet> Planets;

///END MODEL LOADING

CShader boundShader;

glm::mat4 ProjectionMatrix; // matrix for the orthographic projection

//	User Input
int	mouse_x=0, mouse_y=0;
bool LeftPressed = false;
int screenWidth = 600, screenHeight = 600;
bool SwitchCamera = false;
float Throttle;
float Pitch, Yaw, Roll;
bool accelerate, deccelerate;

//Collider drawing
bool showPlayerCollider, showAllColliders;

//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();         //called in winmain to process keyboard input
void idle();		//idle function
void PrintPositions(Object* obj);
/*************    START OF OPENGL FUNCTIONS   ****************/
void display()									
{
	//OpenGL Stuff
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewingMatrix = glm::mat4(1.0f);

	if (SwitchCamera)
	{
		//Camera view from the rocket "cockpit"
		//TODO: Perhaps add both a "cockpit" view and a 3rd person/chase view

		glm::vec3 cameraPosition = rocketShip.GetObjectWorldPosition();
		cameraPosition += (rocketShip.Up() * -5.0f);
		cameraPosition += (rocketShip.Forward() * 3.0f);

		glm::vec3 cameraTarget = rocketShip.GetObjectWorldPosition() + (rocketShip.Up() * 5.0f);
		cameraTarget += (rocketShip.Forward() * 1.0f);

		viewingMatrix = glm::lookAt(cameraPosition, cameraTarget, rocketShip.Forward());
	}
	else
	{
		//Random arbitrary camera in space
		viewingMatrix = glm::lookAt(glm::vec3(0, 0, 100), glm::vec3(0, 0, -50), glm::vec3(0, 1.0, 0));
	}

	//Player rendering
	rocketShip.render(viewingMatrix, ProjectionMatrix, showPlayerCollider || showAllColliders);
	//Render planets
	for (auto it = Planets.begin(); it != Planets.end(); ++it)
		it->render(viewingMatrix, ProjectionMatrix, showAllColliders);
	glFlush();
	glutSwapBuffers();
}

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth=width; screenHeight = height;           // to ensure the mouse coordinates match 
														// we will use these values to set the coordinate system
	glViewport(0,0,width,height);						// Reset The Current Viewport

	//Set the projection matrix
	ProjectionMatrix = glm::perspective(glm::radians(40.0f), (GLfloat)screenWidth/(GLfloat)screenHeight, 1.0f, 200.0f);
}
void init()
{
	//Open GL stuff
	glClearColor(0.0,0.0,0.0,0.0);						//sets the clear colour to yellow
														//glClear(GL_COLOR_BUFFER_BIT) in the display function
														//will clear the buffer to this colour
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	//Object setup
	rocketShip.setupShader("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag");
	rocketShip.init("Models/RocketShip/rocket.obj");

	//Create mars
	Planets.push_back(Planet());
	Planets[0].setupShader("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag");
	Planets[0].init("Models/Planets/Planet_1.obj", glm::vec3(0.0f, -20.0f, -30.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	//Create moon
	Planets.push_back(Planet());
	Planets[1].setupShader("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag");
	Planets[1].init("Models/Planets/Moon_1.obj", glm::vec3(0.0f, -20.0f, -30.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	Planets[1].SetOrbit(&Planets[0], 0.05f, -35.0f);
}

void ApplyGravity()
{
	glm::vec3 playerPosition = rocketShip.GetObjectWorldPosition();
	//Apply gravity to nearest celestial body
	float nearest = 1000.0f;
	int closestPlanetIndex = 0;
	int currentPlanet = 0;
	for (auto it = Planets.begin(); it != Planets.end(); ++it)
	{
		float dist = distance(it->GetObjectWorldPosition(), playerPosition);
		if (dist < nearest)
		{
			nearest = dist;
			closestPlanetIndex = currentPlanet;
		}
		++currentPlanet;
	}
	glm::vec3 planetPosition = Planets[closestPlanetIndex].GetObjectWorldPosition();

	//TODO: Add method to planet to retrieve it's gravity field
	float gravityField = Planets[closestPlanetIndex].GetColliderSphereRadius() * 1.8;
	if (nearest <= gravityField)
	{
		glm::vec3 attractDirection = normalize(planetPosition - playerPosition);
		//TODO: REMOVE MAGIC NUMBER FOR GRAVITY STRENGTH
		rocketShip.Move(attractDirection, 0.01f);
	}
}

void CheckCollisions()
{
	//Check for collisions
	glm::vec3 playerPosition = rocketShip.GetObjectWorldPosition();

	for (auto it = Planets.begin(); it != Planets.end(); ++it)
	{
		glm::vec3 planetPosition = it->GetObjectWorldPosition();
		float playerDistance = distance(planetPosition, playerPosition);
		float colliderRadi = it->GetColliderSphereRadius() + rocketShip.GetColliderSphereRadius();
		if (playerDistance <= colliderRadi)
		{
			glm::vec3 repulseDirection = normalize(playerPosition - planetPosition);
			//TODO: Remove magic number for repulsion amount
			rocketShip.Move(repulseDirection, (playerDistance - colliderRadi) + 0.3f);
		}
	}
}

void PhysicsSimulation() 
{
	//Player rotation is currently broken (try rotating about directional vector)
	//Move player
	rocketShip.Fly(Throttle, glm::vec3(Pitch, Roll, Yaw));
	ApplyGravity();
	CheckCollisions();
}



//BEGIN DEBUG CODE
void PrintPositions(Object* obj)
{
	glm::vec3 pos = obj->GetObjectWorldPosition();
	std::cout << "Object Position: X: " << pos.x << " Y: " << pos.y << " Z: " << pos.z << std::endl;
}

void PrintDirections(Object* obj)
{
	glm::vec3 direction = obj->Up();
	std::cout << "Up: X: " << direction.x << " Y: " << direction.y << " Z: " << direction.z << std::endl;

	direction = obj->Forward();
	std::cout << "FW: X: " << direction.x << " Y: " << direction.y << " Z: " << direction.z << std::endl;

	direction = obj->Side();
	std::cout << "Right: X: " << direction.x << " Y: " << direction.y << " Z: " << direction.z << std::endl << std::endl;
}
//END DEBUG CODE


void special(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_SHIFT_L:
			accelerate = true;
			deccelerate = false;
			break;
		case GLUT_KEY_CTRL_L:
			deccelerate = true;
			accelerate = false;
			break;
	}
}

void specialUp(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_SHIFT_L:
			accelerate = false;
			break;
		case GLUT_KEY_CTRL_L:
			deccelerate = false;
			break;
		case GLUT_KEY_F1:
			SwitchCamera = !SwitchCamera;
			break;
		case GLUT_KEY_F2:
			showPlayerCollider = !showPlayerCollider;
			break;
		case GLUT_KEY_F3:
			showAllColliders = !showAllColliders;
			break;
		case GLUT_KEY_F4:
			PrintPositions(&rocketShip);
			break;
		case GLUT_KEY_F5:
			PrintDirections(&rocketShip);
			break;
	}
}

void KeyDown(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'w':
			Pitch = -1.0f;
			break;
		case 's':
			Pitch = 1.0f;
			break;
		case 'a':
			Yaw = 1.0f;
			break;
		case 'd':
			Yaw = -1.0f;
			break;
		case 'q':
			Roll = -1.0f;
			break;
		case 'e':
			Roll = 1.0f;
			break;
	}
}
void KeyUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
	case 's':
		Pitch = 0.0f;
		break;
	case 'a':
	case 'd':
		Yaw = 0.0f;
		break;
	case 'q':
	case 'e':
		Roll = 0.0f;
		break;
	}
}

void processKeys()
{
	//"Forward" axis is Y
	if (accelerate && Throttle+0.01f < 1.0f)
	{
		Throttle += 0.01f;
	}
	if (deccelerate && Throttle-0.01f > 0.0f)
	{
		Throttle -= 0.01f;
	}
}

void idle()
{
	PhysicsSimulation();
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
