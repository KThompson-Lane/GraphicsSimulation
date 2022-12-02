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
int screenWidth = 600, screenHeight = 600;
bool SwitchCamera = false;
float Throttle;
float Pitch, Yaw, Roll;
float VerticleThrottle;

bool accelerate, deccelerate;
int CameraIndex = 0;
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

	switch(CameraIndex)
	{
		case 0:
			//Chase camera view (default)
			viewingMatrix = mainCamera.GetViewMatrix();
			break;
		case 1:
			//cockpit view
			glm::vec3 cameraPosition = rocketShip.GetObjectWorldPosition();
			cameraPosition += (rocketShip.Up() * 0.08f);
			cameraPosition += (rocketShip.Forward() * 0.09f);

			glm::vec3 cameraTarget = rocketShip.GetObjectWorldPosition();
			cameraTarget += (rocketShip.Forward() * 2.0f);

			glm::vec3 cameraDirection = glm::normalize(cameraPosition - cameraTarget);
			glm::vec3 cameraRight = glm::normalize(glm::cross(rocketShip.Up(), cameraDirection));
			glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
			viewingMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
			
			break;
		default:
			//Looking down at sun POV
			viewingMatrix = glm::lookAt(glm::vec3(0.0, 500.0f, 0.0), glm::vec3(0.0, -50.0f, 0.0), glm::vec3(0, 0.0, 1.0));
	}

	//Player rendering
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
	rocketShip.setupShader("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformationsWithDisplacement.frag");
	rocketShip.init("Models/Ship/Ship.obj");
	rocketShip.Move(glm::vec3(1.0, 0.0, 0.0), 120);
	rocketShip.Move(glm::vec3(0.0, 0.0, 1.0), 20);
	rocketShip.Rotate(0.0, 225.0f ,0.0);

	//Create ship spotlight
	playerSpot.ambient = { 0.8, 0.8, 0.8 };
	playerSpot.diffuse = { 0.8, 0.8, 0.8 };
	playerSpot.specular = glm::vec3(1.0);
	playerSpot.position = glm::vec3(0.0f, 0.0f, 0.0f);

	playerSpot.constant = 1.0f;
	playerSpot.linear = 0.35;
	playerSpot.quadratic = 0.44;

	playerSpot.position = rocketShip.GetObjectWorldPosition() + (rocketShip.Up() * 0.15f);
	playerSpot.direction = rocketShip.Forward();
	playerSpot.cutOff = 12.5f;

	//Create star
	Bodies.push_back(CelestialBody(0));
	Bodies[0].setupShader("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformationsWithDisplacement.frag");
	Bodies[0].init("Models/Bodies/Star/Star.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	//Create Delmar
	Bodies.push_back(CelestialBody(1));
	Bodies[1].setupShader("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformationsWithDisplacement.frag");
	Bodies[1].init("Models/Bodies/Delmar/Delmar.obj", glm::vec3(100.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	Bodies[1].SetOrbit(0, 0.001f, 100.0f);

	//Create moon
    Bodies.push_back(CelestialBody(2));
	Bodies[2].setupShader("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformationsWithDisplacement.frag");
	Bodies[2].init("Models/Bodies/Moon/Moon.obj", glm::vec3(130.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	Bodies[2].SetOrbit(1, 0.01f, 30.0f);

	//Setup Camera
	mainCamera.SetCameraView(glm::vec3(120.0f, 0.0, 30.0f), rocketShip.GetObjectWorldPosition(), glm::vec3(0.0, 1.0, 0.0));
}

void UpdateOrbits()
{
	for (int i = 0; i < Bodies.size(); i++)
	{
		if (Bodies[i].orbitingBodyIndex != -1)
		{
			int orbitindex = Bodies[i].orbitingBodyIndex;
			Bodies[i].UpdateOrbit(Bodies[orbitindex].GetObjectWorldPosition(), deltaTime);
		}
	}
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

	glm::vec3 attractDirection = normalize(planetPosition - playerPosition);
	//calculate gravity strength
	float m1 = rocketShip.GetColliderSphereRadius();
	float m2 = Bodies[closestPlanetIndex].GetColliderSphereRadius();

	float strength = G * ((m1 * m2) / (nearest * nearest));
	rocketShip.Move(attractDirection, strength * deltaTime);
	
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

	//Finally update ship light to match new position
	playerSpot.position = rocketShip.GetObjectWorldPosition() + (rocketShip.Up() * -0.15f);
}

void PhysicsSimulation() 
{
	float currentTime = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = currentTime - lastFrameTime;
	lastFrameTime = currentTime;
	UpdateOrbits();
	ApplyGravity();
	CheckCollisions();
	PlayerMovement();
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
			if (++CameraIndex == 3)
				CameraIndex = 0;
			break;
		case GLUT_KEY_F2:
			showPlayerCollider = !showPlayerCollider;
			break;
		case GLUT_KEY_F3:
			showAllColliders = !showAllColliders;
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
		playerSpot.active = !playerSpot.active;
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
void UpdateCamera() 
{
	//Get camera and pivot positions
	glm::vec4 pivot(rocketShip.GetObjectWorldPosition(), 1.0);
	//glm::vec4 position(mainCamera.GetEye(), 1.0);
	glm::vec4 position(rocketShip.GetObjectWorldPosition() - (mainCamera.GetViewDir() * zoom), 1.0f);

	
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
void ScrollWheel(int wheel, int dir, int x, int y)
{
	dir = -dir;
	if (zoom + (dir * 0.5f) > 2.0f && zoom + (dir * 0.5f) < 20.0f)
	{
		zoom += (dir * 0.5f);
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
