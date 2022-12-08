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

static const float G = 0.00069420f;
Player rocketShip = Player();
CelestialBody body = CelestialBody("test");

vector<CelestialBody> Bodies;
//Lighting
#include "Light/Light.h"
vector<PointLight> lights;
SpotLight playerSpot;

//Camera
#include "Camera/Camera.h"
Camera mainCamera;
int lastMouse_x = 0, lastMouse_y = 0;

///END MODEL LOADING
CShader boundShader;

glm::mat4 ProjectionMatrix; // matrix for the orthographic projection

//	User Input
int	mouse_x=0, mouse_y=0;
bool MiddlePressed = false;
float zoom = 10.0f;
std::pair<float, float> zoomBounds;
int screenWidth = 600, screenHeight = 600;
bool SwitchCamera = false;
float Throttle;
float Pitch, Yaw, Roll;
float VerticleThrottle;

bool accelerate, deccelerate;
bool cockpitCam;
Object* focusedObject;

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

/*************    START OF OPENGL FUNCTIONS   ****************/
void display()									
{
	//OpenGL Stuff
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewingMatrix = glm::mat4(1.0f);

	if (cockpitCam)
	{
		//cockpit view
		glm::vec3 cameraPosition = rocketShip.transform->position;
		cameraPosition += (rocketShip.transform->Up() * 0.08f);
		cameraPosition += (rocketShip.transform->Forward() * 0.09f);

		glm::vec3 cameraTarget = rocketShip.transform->position;
		cameraTarget += (rocketShip.transform->Forward() * 2.0f);

		glm::vec3 cameraDirection = glm::normalize(cameraPosition - cameraTarget);
		glm::vec3 cameraRight = glm::normalize(glm::cross(rocketShip.transform->Up(), cameraDirection));
		glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
		viewingMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
	}
	else
	{
		viewingMatrix = mainCamera.GetViewMatrix();
	}

	//Before rendering update playerSpot position;
	glm::mat4 lightMat = glm::translate(glm::mat4(1.0), rocketShip.transform->position);
	lightMat *= glm::toMat4(rocketShip.transform->rotation);
	playerSpot.position = viewingMatrix * lightMat[3];
	playerSpot.direction = glm::normalize(viewingMatrix * lightMat[2]);

	rocketShip.render(viewingMatrix, ProjectionMatrix, showPlayerCollider || showAllColliders, lights, playerSpot);

	//Render planets
	for (auto it = Bodies.begin(); it != Bodies.end(); ++it)
	{
		it->render(viewingMatrix, ProjectionMatrix, showAllColliders, lights, playerSpot);
	}
	glFlush();
	glutSwapBuffers();
}

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth=width; screenHeight = height;           // to ensure the mouse coordinates match 
														// we will use these values to set the coordinate system
	glViewport(0,0,width,height);						// Reset The Current Viewport

	//Set the projection matrix
	ProjectionMatrix = glm::perspective(glm::radians(25.0f), (GLfloat)screenWidth/(GLfloat)screenHeight, 0.001f, 800.0f);
}
void init()
{
	//Open GL stuff
	glClearColor(0.0,0.0,0.0,0.0);						//sets the clear colour to black
														//glClear(GL_COLOR_BUFFER_BIT) in the display function
														//will clear the buffer to this colour
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	lights.push_back(PointLight());
	//Create light for the Star
	lights[0].ambient = {0.8, 0.8, 0.8};
	lights[0].diffuse = { 0.8, 0.8, 0.8 };
	lights[0].specular = glm::vec3(1.0);
	lights[0].position = glm::vec3(0.0f, 0.0f, 0.0f);

	lights[0].constant = 1.0f;
	lights[0].linear = 0.007;
	lights[0].quadratic = 0.0002;

	//Player setup
	rocketShip.setupShader("BasicView", "glslfiles/basicTransformationsWithDisplacement.vert", "glslfiles/basicTransformationsWithDisplacement.frag");
	rocketShip.init("Models/Ship/Ship.obj");
	rocketShip.transform->Move(glm::vec3(1.0, 0.0, 0.0), 120.0f);
	rocketShip.transform->Move(glm::vec3(0.0, 0.0, 1.0), 20.0f);
	rocketShip.transform->Rotate(0.0, 225.0f ,0.0);
	rocketShip.AddBoxCollider();

	//Create ship spotlight
	playerSpot.ambient = { 0.8, 0.8, 0.8 };
	playerSpot.diffuse = { 0.8, 0.8, 0.8 };
	playerSpot.specular = glm::vec3(1.0);

	playerSpot.constant = 1.0f;
	playerSpot.linear = 0.35;
	playerSpot.quadratic = 0.44;
	playerSpot.cutOff = 12.5f;

	//Create star
	Bodies.push_back(CelestialBody(string("star")));
	Bodies[0].setupShader("BasicView", "glslfiles/basicTransformationsWithDisplacement.vert", "glslfiles/basicTransformationsWithDisplacement.frag");
	Bodies[0].init("Models/Bodies/Star/Star.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	Bodies[0].AddSphereCollider();

	//Create Delmar
	Bodies.push_back(CelestialBody(string("delmar")));
	Bodies[1].setupShader("BasicView", "glslfiles/basicTransformationsWithDisplacement.vert", "glslfiles/basicTransformationsWithDisplacement.frag");
	Bodies[1].init("Models/Bodies/Delmar/Delmar.obj", glm::vec3(100.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	Bodies[1].AddSphereCollider();
	Bodies[1].SetOrbit(0, 0.0003f, 100.0f);

	//Create moon
    Bodies.push_back(CelestialBody(string("moon")));
	Bodies[2].setupShader("BasicView", "glslfiles/basicTransformationsWithDisplacement.vert", "glslfiles/basicTransformationsWithDisplacement.frag");
	Bodies[2].init("Models/Bodies/Moon/Moon.obj", glm::vec3(130.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	Bodies[2].AddSphereCollider();
	Bodies[2].SetOrbit(1, 0.003f, 30.0f);
	

	//Setup Camera
	focusedObject = &rocketShip;
	mainCamera.SetCameraView(glm::vec3(120.0f, 0.0, 30.0f), rocketShip.transform->position, glm::vec3(0.0, 1.0, 0.0));
}

void UpdateCamera()
{
	zoomBounds = focusedObject->GetMinMaxZoom();
	glm::vec3 focusPosition = focusedObject->transform->position;
	
	//Check zoom is within boundaries
	if (zoom < zoomBounds.first)
	{
		zoom = zoomBounds.first;
	}
	else if (zoom > zoomBounds.second)
	{
		zoom = zoomBounds.second;
	}

	//Get camera and pivot positions
	glm::vec4 pivot = glm::vec4(focusPosition, 1.0);
	glm::vec4 position = glm::vec4(focusPosition - (mainCamera.GetViewDir() * zoom), 1.0f);

	//Calculate rotation amount
	float deltaX = (2 * PI / screenWidth); //Left -> right = 360
	float deltaY = (PI / screenHeight); //Top -> bottom = 180
	float yawIncrement = (lastMouse_x - mouse_x) * deltaX;
	float pitchIncrement = (lastMouse_y - mouse_y) * deltaY;
	glm::quat cameraYaw = glm::angleAxis(yawIncrement, mainCamera.GetUpVector());
	glm::quat cameraPitch = glm::angleAxis(pitchIncrement, mainCamera.GetRightVector());

	glm::quat cameraOrientation = cameraYaw * cameraPitch;
	position = (glm::toMat4(cameraOrientation) * (position - pivot)) + pivot;

	mainCamera.SetCameraView(position, pivot, glm::vec3(0.0, 1.0, 0.0));
	
	lastMouse_x = mouse_x;
	lastMouse_y = mouse_y;
}

void DestroyPlayer() 
{
	cockpitCam = false;
	playerSpot.active = false;
}

void UpdateOrbits()
{
	for (int i = 0; i < Bodies.size(); i++)
	{
		if (Bodies[i].orbitingBodyIndex != -1)
		{
			int orbitindex = Bodies[i].orbitingBodyIndex;
			Bodies[i].UpdateOrbit(Bodies[orbitindex].transform->position, deltaTime);
		}
	}
}

void ApplyGravity()
{
	glm::vec3 playerPosition = rocketShip.transform->position;

	//Apply gravity to nearest celestial body
	float nearest = distance(Bodies[0].transform->position, playerPosition);
	int closestPlanetIndex = 0;
	int currentPlanet = 0;
	for (auto it = Bodies.begin(); it != Bodies.end(); ++it)
	{
		float dist = distance(it->transform->position, playerPosition);
		if (dist < nearest)
		{
			nearest = dist;
			closestPlanetIndex = currentPlanet;
		}
		++currentPlanet;
	}
	glm::vec3 planetPosition = Bodies[closestPlanetIndex].transform->position;

	glm::vec3 attractDirection = normalize(planetPosition - playerPosition);
	//calculate gravity strength
	float m1 = rocketShip.GetColliderSphereRadius();
	float m2 = Bodies[closestPlanetIndex].GetColliderSphereRadius();

	float strength = G * ((m1 * m2) / (nearest * nearest));
	rocketShip.AddForce(attractDirection * strength);
}

void CheckCollisions()
{
	//Check for collisions

	if (!rocketShip.landed && !rocketShip.destroyed)
	{
		for (auto it = Bodies.begin(); it != Bodies.end(); ++it)
		{
			if (rocketShip.CheckCollision(*it))
			{
				//If player collides set focused object to collision object
				focusedObject = &(*it);
				if (rocketShip.destroyed)
				{
					DestroyPlayer();
				}
			}
		}
	}
}

void PlayerMovement()
{	
	//Calculate rotation increments based on player input
	if (!rocketShip.landed && !rocketShip.destroyed)
	{
		float yawInput = (Yaw * rocketShip.GetRotationSpeed()) * deltaTime;
		float pitchInput = (Pitch * rocketShip.GetRotationSpeed()) * deltaTime;
		float rollInput = (Roll * rocketShip.GetRotationSpeed()) * deltaTime;
		rocketShip.transform->Rotate(pitchInput, yawInput, rollInput);

		rocketShip.AddForce(rocketShip.transform->Forward() * (Throttle * rocketShip.GetSpeed()));
		//Replace 0.0003f with a const value for vertical acceleration force;
		rocketShip.AddForce(rocketShip.transform->Up() * (VerticleThrottle * 0.00008f));
	}
	else if (VerticleThrottle == 1.0f && !rocketShip.destroyed)
	{
		rocketShip.TakeOff();
		focusedObject = &rocketShip;
	}

	rocketShip.UpdatePosition(deltaTime);
}

void PhysicsSimulation() 
{
	float currentTime = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = currentTime - lastFrameTime;
	lastFrameTime = currentTime;
	
	//std::cout << "Player orientation: " << rocketShip.transform->rotation.x << " " << rocketShip.transform->rotation.y << " " << rocketShip.transform->rotation.z << " " << rocketShip.transform->rotation.w << std::endl;
	UpdateOrbits();
	if(!rocketShip.landed)
		ApplyGravity();
	PlayerMovement();
	CheckCollisions();
}

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
			cockpitCam = !cockpitCam;
			break;
		case GLUT_KEY_F2:
			showPlayerCollider = !showPlayerCollider;
			break;
		case GLUT_KEY_F3:
			showAllColliders = !showAllColliders;
			break;
		case GLUT_KEY_F4:
			//DEBUG CODE
			rocketShip.Crash();
			break;
	}
}

void KeyDown(unsigned char key, int x, int y)
{
	switch (std::tolower(key))
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
	switch (std::tolower(key))
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
	case 'f':
		if (!rocketShip.destroyed)
			playerSpot.active = !playerSpot.active;
		break;
	case 'p':
		if (rocketShip.destroyed)
		{
			//respawn player
			Throttle = 0.0f;
			rocketShip.Reset(glm::vec3(120.0, 0.0, 20.0));
			focusedObject = &rocketShip;
		}
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

void updateMousePos(int x, int y)
{
	if (MiddlePressed)
	{
		mouse_x = x;
		mouse_y = y;
	}
}

void mouseInput(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_MIDDLE_BUTTON:
		if (state == GLUT_DOWN)
		{
			lastMouse_x = x;
			lastMouse_y = y;
			mouse_x = x;
			mouse_y = y;
			MiddlePressed = true;
		}
		else
			MiddlePressed = false;
	}
}

void ScrollWheel(int wheel, int dir, int x, int y)
{
	dir = -dir;
	float zoomInc = zoomBounds.first * 0.5;
	if (zoom + (dir * zoomInc) > zoomBounds.first && zoom + (dir * zoomInc) < zoomBounds.second)
	{
		zoom += (dir * zoomInc);
	}
}
void idle()
{
	PhysicsSimulation();
	processKeys();
	UpdateCamera();
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

	//Handle mouse input 
	glutMotionFunc(updateMousePos);
	glutMouseFunc(mouseInput);
	glutMouseWheelFunc(ScrollWheel);
	glutIdleFunc(idle);

	//starts the main loop. Program loops and calls callback functions as appropriate.
	glutMainLoop();

	return 0;
}
