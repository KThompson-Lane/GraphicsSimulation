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
#include "Object/CelestialBody.h"

const float G = 0.069420f;
Player rocketShip = Player();

vector<CelestialBody> Bodies;
vector<PointLight> lights;
///END MODEL LOADING

//Lighting
#include "Light/Light.h"
PointLight light;

CShader boundShader;

glm::mat4 ProjectionMatrix; // matrix for the orthographic projection

//	User Input
int	mouse_x=0, mouse_y=0;
bool LeftPressed = false;
int screenWidth = 600, screenHeight = 600;
bool SwitchCamera = false;
float Throttle;
float Pitch, Yaw, Roll;
float VerticleThrottle;

bool accelerate, deccelerate;
int CameraIndex;
//Collider drawing
bool showPlayerCollider, showAllColliders;

//Physics code
float deltaTime;
float lastFrameTime = 1000.0f;

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

	switch(CameraIndex)
	{
		case 0:
			//Chase camera view (default)
			glm::vec3 cameraPosition = rocketShip.GetObjectWorldPosition();
			cameraPosition += (rocketShip.Up() * 1.5f);
			cameraPosition += (rocketShip.Forward() * -5.0f);

			glm::vec3 cameraTarget = rocketShip.GetObjectWorldPosition();
			cameraTarget += (rocketShip.Forward() * 5.0f);

			glm::vec3 cameraDirection = glm::normalize(cameraPosition - cameraTarget);
			glm::vec3 cameraRight = glm::normalize(glm::cross(rocketShip.Up(), cameraDirection));
			glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

			viewingMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
			break;
		case 1:
			//cockpit view

			cameraPosition = rocketShip.GetObjectWorldPosition();
			cameraPosition += (rocketShip.Up() * 0.15f);
			cameraPosition += (rocketShip.Forward() * 0.19f);

			cameraTarget = rocketShip.GetObjectWorldPosition();
			cameraTarget += (rocketShip.Forward() * 2.0f);

			cameraDirection = glm::normalize(cameraPosition - cameraTarget);
			cameraRight = glm::normalize(glm::cross(rocketShip.Up(), cameraDirection));
			cameraUp = glm::cross(cameraDirection, cameraRight);

			viewingMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
			
			break;
		default:
			//Random arbitrary camera in space
			viewingMatrix = glm::lookAt(glm::vec3(0, 0, 100), glm::vec3(0, 0, -50), glm::vec3(0, 1.0, 0));
	}

	//Player rendering
	rocketShip.render(viewingMatrix, ProjectionMatrix, showPlayerCollider || showAllColliders, lights);
	//Render planets

	for (auto it = Bodies.begin(); it != Bodies.end(); ++it)
		it->render(viewingMatrix, ProjectionMatrix, showAllColliders, lights);
	glFlush();
	glutSwapBuffers();
}

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth=width; screenHeight = height;           // to ensure the mouse coordinates match 
														// we will use these values to set the coordinate system
	glViewport(0,0,width,height);						// Reset The Current Viewport

	//Set the projection matrix
	ProjectionMatrix = glm::perspective(glm::radians(25.0f), (GLfloat)screenWidth/(GLfloat)screenHeight, 0.001f, 400.0f);
}
void init()
{
	//Open GL stuff
	glClearColor(0.0,0.0,0.0,0.0);						//sets the clear colour to yellow
														//glClear(GL_COLOR_BUFFER_BIT) in the display function
														//will clear the buffer to this colour
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	lights.push_back(PointLight());
	//Create simple light
	lights[0].ambient = {0.8, 0.8, 0.8};
	lights[0].diffuse = { 0.8, 0.8, 0.8 };
	lights[0].specular = glm::vec3(1.0);
	lights[0].position = glm::vec3(35.0f, 0, 80.0f);

	lights[0].constant = 1.0f;
	lights[0].linear = 0.007;
	lights[0].quadratic = 0.0002;

	//Object setup
	rocketShip.setupShader("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformationsWithDisplacement.frag");
	rocketShip.init("Models/Ship/Ship.obj");
	rocketShip.Move(glm::vec3(1.0, 0.0, 0.0), 45.0f);
	rocketShip.Move(glm::vec3(0.0, 0.0, 1.0), 80.0f);
	rocketShip.Rotate(0.0, -90.0f, 0.0);

	//Create Delmar
	Bodies.push_back(CelestialBody());
	Bodies[0].setupShader("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformationsWithDisplacement.frag");
	Bodies[0].init("Models/Bodies/Delmar/Delmar.obj", glm::vec3(0.0, 0.0f, 0.0), glm::vec3(0.0f, 0.0f, 0.0f), 1.8);
	Bodies[0].Move(glm::vec3(1.0, 0.0, 0.0), 30.0f);
	Bodies[0].Move(glm::vec3(0.0, 0.0, 1.0), 80.0f);

	//Create moon
    Bodies.push_back(CelestialBody());
	Bodies[1].setupShader("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformationsWithDisplacement.frag");
	Bodies[1].init("Models/Bodies/Moon/Moon.obj", glm::vec3(30.0f, 0.0f, 80.0f), glm::vec3(0.0f, 0.0f, 0.0f), 2.5);
	Bodies[1].SetOrbit(&Bodies[0], 0.0005f, -30.0f);

}

void ApplyGravity()
{
	glm::vec3 playerPosition = rocketShip.GetObjectWorldPosition();
	//Apply gravity to nearest celestial body
	float nearest = 1000.0f;
	int closestPlanetIndex = 0;
	int currentPlanet = 0;
	for (auto it = Bodies.begin(); it != Bodies.end(); ++it)
	{
		float dist = distance(it->GetObjectWorldPosition(), playerPosition);
		if (dist < nearest)
		{
			nearest = dist;
			closestPlanetIndex = currentPlanet;
		}
		++currentPlanet;
	}
	glm::vec3 planetPosition = Bodies[closestPlanetIndex].GetObjectWorldPosition();

	if (nearest <= Bodies[closestPlanetIndex].GetGravityDistance())
	{
		glm::vec3 attractDirection = normalize(planetPosition - playerPosition);
		//calculate gravity strength
		float m1 = rocketShip.GetColliderSphereRadius();
		float m2 = Bodies[closestPlanetIndex].GetColliderSphereRadius();

		float strength = G * ((m1 * m2) / (nearest * nearest));
		rocketShip.Move(attractDirection, strength);
	}
}
void ApplyOrbits()
{
	for (auto it = Bodies.begin(); it != Bodies.end(); ++it)
	{
		if (it->orbitingBody != nullptr)
		{
			it->orbitAmount += (it->orbitalSpeed*deltaTime);
			if (it->orbitAmount >= 360.0f)
			{
				it->orbitAmount -= 360.0f;
			}
			else if (it->orbitAmount < 0.0f)
			{
				it->orbitAmount += 360.0f;
			}

			//Try to do this without matrix operations if possible. Quaternions?

			//Create new matrix for orbiting at the position of the body we are orbiting
			glm::mat4 orbitMatrix = glm::translate(glm::mat4(1.0), it->orbitingBody->GetObjectWorldPosition());

			orbitMatrix = glm::rotate(orbitMatrix, glm::radians(it->orbitAmount), glm::vec3(0.0, 1.0, 0.0));
			orbitMatrix = glm::translate(orbitMatrix, glm::vec3(it->orbitDistance, 0.0, 0.0));

			//Rotate by X then rotate by Y 
			glm::vec3 objectPosition;
			objectPosition.x = orbitMatrix[3][0];
			objectPosition.y = orbitMatrix[3][1];
			objectPosition.z = orbitMatrix[3][2];
			it->SetPosition(objectPosition);
		}
	}
}

void CheckCollisions()
{
	//Check for collisions
	glm::vec3 playerPosition = rocketShip.GetObjectWorldPosition();

	for (auto it = Bodies.begin(); it != Bodies.end(); ++it)
	{
		glm::vec3 planetPosition = it->GetObjectWorldPosition();
		float playerDistance = distance(planetPosition, playerPosition);
		float colliderRadi = it->GetColliderSphereRadius() + rocketShip.GetColliderSphereRadius();
		if (playerDistance <= colliderRadi)
		{
			glm::vec3 repulseDirection = normalize(playerPosition - planetPosition);

			float Rp = playerDistance - rocketShip.GetColliderSphereRadius();
			float Rc = playerDistance - it->GetColliderSphereRadius();
			float P = playerDistance - (Rp + Rc);

			//TODO: If implementing newtonian physics replace this with a real equation based on velocity.
			rocketShip.Move(repulseDirection, P);
		}
	}
}

void PlayerMovement()
{
	//Calculate rotation increments based on player input
	float yawInput = (Yaw * rocketShip.GetRotationSpeed()) * deltaTime;
	float pitchInput = (Pitch * rocketShip.GetRotationSpeed()) * deltaTime;
	float rollInput = (Roll * rocketShip.GetRotationSpeed()) * deltaTime;
	rocketShip.Rotate(pitchInput, yawInput, rollInput);

	//Calculate player forward thrust
	rocketShip.Move(rocketShip.Forward(), (Throttle * rocketShip.GetSpeed()) * deltaTime);
	//Calculate player vertical thrust
	rocketShip.Move(rocketShip.Up(), (VerticleThrottle * 0.003) * deltaTime);
}

void PhysicsSimulation() 
{
	float currentTime = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = currentTime - lastFrameTime;
	lastFrameTime = currentTime;
	//ApplyGravity();
	ApplyOrbits();
	CheckCollisions();
	PlayerMovement();
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
			if (++CameraIndex == 3)
				CameraIndex = 0;
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
		case ' ':
			VerticleThrottle = 1.0f;
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
	case ' ':
		VerticleThrottle = 0.0f;
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
