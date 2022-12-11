#include <iostream>
using namespace std;

//------------ OpenGL ------------
#include "GL\glew.h"
#include "GL\wglew.h"
#pragma comment(lib, "glew32.lib")
//--------------------------------

//-------- GLM related includes --------
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"
#include "GL\freeglut.h"
#include "Images\FreeImage.h"
//--------------------------------------

//----------------------- Physics constants -----------------------
static const double G = 0.0000113769f;	//	Gravitational constant 
float deltaTime;						//	Timestep float
float lastFrameTime = 1000.0f;			//	Stores last frametime
//-----------------------------------------------------------------

//--- Object code and objects ---
#include "Object/Object.h"
#include "Object/Player.h"
#include "Object/CelestialBody.h"
#include "Object/Craft.h"
Player rocketShip = Player();
vector<CelestialBody> Bodies;
Craft satellite = Craft();
//-------------------------------

//--- Lighting code and objects ---
#include "Light/Light.h"
vector<PointLight> lights;
SpotLight playerSpot;
//---------------------------------

//-------------------- Camera code and objects ---------------------
#include "Camera/Camera.h"
Camera mainCamera;
int lastMouse_x = 0, lastMouse_y = 0;	//	Last mouse position
int cameraIndex = 0;					//	Index for planet focus
Object* focusedObject;					//	Object focused by camera
float zoom = 10.0f;
std::pair<float, float> zoomBounds;		//	Camera min/max zoom
bool cockpitCam;
//------------------------------------------------------------------

//------------- Display code --------------
glm::mat4 ProjectionMatrix;
int screenWidth = 600, screenHeight = 600;
//-----------------------------------------

//--------------------------- User input code ---------------------------
int	mouse_x=0, mouse_y=0;					//  Current mouse coordinates
bool MiddlePressed = false;
float Pitch, Yaw, Roll;						//	Floats for rotation input
float VerticalThrottle;
bool accelerate;
bool showPlayerCollider, showAllColliders;	//	Collider drawing
//-----------------------------------------------------------------------

//----------------------------- Function declarations -----------------------------
void Display();							//	Called when a new frame is needed
void Reshape(int width, int height);	//	Called when the window is resized
void Init();							//	Called in main to initialize simulation
void Idle();							//	Idle function (Also requests new frames)
//---------------------------------------------------------------------------------

#pragma region Function Definitions

/*
 *	Method	: Display
 *	Scope	: Private
 *	Purpose	: Renders a new frame to be displayed by GLUT
 *	Returns	: void
 */
void Display()									
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//Clear buffers

	glm::mat4 viewingMatrix = glm::mat4(1.0f);			//Create viewing matrix 

	
	//Use cockpitCam boolean to determine what type of viewing matrix to use
	if (cockpitCam)
	{
		//	Creates cockpit viewing matrix using player position and direction

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
		//	Creates default viewing matrix using camera focus
		viewingMatrix = mainCamera.GetViewMatrix();
	}

	//	Before rendering update position of player spotlight:
	glm::mat4 lightMat = glm::translate(glm::mat4(1.0), rocketShip.transform->position);
	lightMat *= glm::toMat4(rocketShip.transform->rotation);
	playerSpot.position = viewingMatrix * lightMat[3];
	playerSpot.direction = glm::normalize(viewingMatrix * lightMat[2]);

	//	Also update satellite light position:
	lights[1].position = (satellite.transform->position +satellite.transform->Forward() * 0.53f);

	//	Render player object
	rocketShip.render(viewingMatrix, ProjectionMatrix, showPlayerCollider || showAllColliders, lights, playerSpot);

	//	Render planets
	for (auto it = Bodies.begin(); it != Bodies.end(); ++it)
	{
		it->render(viewingMatrix, ProjectionMatrix, showAllColliders, lights, playerSpot);
	}

	//	Render satellite
	satellite.render(viewingMatrix, ProjectionMatrix, showAllColliders, lights, playerSpot);

	glFlush();
	glutSwapBuffers();
}

/*
 *	Method	: Reshape
 *	Scope	: Private
 *	Purpose	: Recreates the projection matrix 
 *			  upon window resize
 *	Returns	: void
 */
void Reshape(int width, int height)
{
	screenWidth=width; screenHeight = height;           //	Set screen dimension variables
	glViewport(0,0,width,height);						//	Reset The Current Viewport

	//Recreate the projection matrix with our new screen dimensions
	ProjectionMatrix = glm::perspective(glm::radians(25.0f), (GLfloat)screenWidth/(GLfloat)screenHeight, 0.001f, 800.0f);
}

/*
 *	Method	: Init
 *	Scope	: Private
 *	Purpose	: Initializes the simulation
 *	Returns	: void
 */
void Init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);	//	Set clear colour (black)
	glEnable(GL_DEPTH_TEST);			//	Enable depth-testing
	glEnable(GL_CULL_FACE);				//	Enable backface culling

	//	Create point light for the Star
	lights.push_back(PointLight());
	lights[0].ambient = {0.8, 0.8, 0.8};
	lights[0].diffuse = { 0.8, 0.8, 0.8 };
	lights[0].specular = glm::vec3(1.0);
	lights[0].position = glm::vec3(0.0f, 0.0f, 0.0f);

	//	Values determining light attenuation
	lights[0].constant = 1.0f;			
	lights[0].linear = 0.007;
	lights[0].quadratic = 0.0002;

	//	Player object initialization with box collider
	rocketShip.setupShader("BasicView", "glslfiles/basicTransformationsWithDisplacement.vert", "glslfiles/basicTransformationsWithDisplacement.frag");
	rocketShip.init("Models/Ship/Ship.obj");
	rocketShip.transform->Move(glm::vec3(120.0f, 0.0f, 20.0f));
	rocketShip.transform->Rotate(0.0, 225.0f ,0.0);
	rocketShip.AddBoxCollider();

	//	Create ship spotlight
	playerSpot.ambient = { 0.8, 0.8, 0.8 };
	playerSpot.diffuse = { 0.8, 0.8, 0.8 };
	playerSpot.specular = glm::vec3(1.0);

	playerSpot.constant = 1.0f;
	playerSpot.linear = 0.35;
	playerSpot.quadratic = 0.44;
	playerSpot.cutOff = 12.5f;

	//	Create star object with sphere collider
	Bodies.push_back(CelestialBody(string("star")));
	Bodies[0].setupShader("BasicView", "glslfiles/basicTransformationsWithDisplacement.vert", "glslfiles/basicTransformationsWithDisplacement.frag");
	Bodies[0].init("Models/Bodies/Star/Star.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	Bodies[0].AddSphereCollider();

	//	Create planet 'Delmar' with sphere collider orbiting the star
	Bodies.push_back(CelestialBody(string("delmar")));
	Bodies[1].setupShader("BasicView", "glslfiles/basicTransformationsWithDisplacement.vert", "glslfiles/basicTransformationsWithDisplacement.frag");
	Bodies[1].init("Models/Bodies/Delmar/Delmar.obj", glm::vec3(100.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	Bodies[1].AddSphereCollider();
	Bodies[1].SetOrbit(0, 0.0003f);

	//	Create a moon with sphere collider orbiting 'Delmar'
    Bodies.push_back(CelestialBody(string("moon")));
	Bodies[2].setupShader("BasicView", "glslfiles/basicTransformationsWithDisplacement.vert", "glslfiles/basicTransformationsWithDisplacement.frag");
	Bodies[2].init("Models/Bodies/Moon/Moon.obj", glm::vec3(30.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	Bodies[2].AddSphereCollider();
	Bodies[2].SetOrbit(1, 0.003f);
	
	//	Create planet 'Orion' with sphere collider orbiting the star
	Bodies.push_back(CelestialBody(string("orion")));
	Bodies[3].setupShader("BasicView", "glslfiles/basicTransformationsWithDisplacement.vert", "glslfiles/basicTransformationsWithDisplacement.frag");
	Bodies[3].init("Models/Bodies/Orion/Orion.obj", glm::vec3(-110.0f, 0.0f, -60.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	Bodies[3].AddSphereCollider();
	Bodies[3].SetOrbit(0, 0.0002f);

	//	Create Satellite object with sphere collider orbiting 'Orion'
	satellite.setupShader("BasicView", "glslfiles/basicTransformationsWithDisplacement.vert", "glslfiles/basicTransformationsWithDisplacement.frag");
	satellite.init("Models/Bodies/Satellite/Satellite.obj", glm::vec3(-20.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	satellite.AddSphereCollider();
	satellite.SetOrbit(3, 0.02f);

	//	Create point light for the Satellite
	lights.push_back(PointLight());
	lights[1].ambient = { 0.1, 0.1, 0.8 };
	lights[1].diffuse = { 0.1, 0.1, 0.8 };
	lights[1].specular = glm::vec3(1.0);
	lights[1].position = satellite.transform->position;

	//	Much weaker 'strength' than star point light
	lights[1].constant = 1.0f;
	lights[1].linear = 0.7;
	lights[1].quadratic = 1.8;

	//	Camera initialization default focus on player
	focusedObject = &rocketShip;
	mainCamera.SetCameraView(glm::vec3(120.0f, 0.0, 30.0f), rocketShip.transform->position, glm::vec3(0.0, 1.0, 0.0));

	//	Print object masses
	std::cout << "Object Masses:" << std::endl;
	std::cout << "Spaceship Mass: " << rocketShip.GetMass() << std::endl;
	std::cout << "Star Mass: " << Bodies[0].GetMass() << std::endl;
	std::cout << "Delmar Mass: " << Bodies[1].GetMass() << std::endl;
	std::cout << "Moon Mass: " << Bodies[2].GetMass() << std::endl;
	std::cout << "Orion Mass: " << Bodies[3].GetMass() << std::endl;
	std::cout << "Satellite Mass: " << satellite.GetMass() << std::endl;
}

/*
 *	Method	: UpdateCamera
 *	Scope	: Private
 *	Purpose	: Updates the main camera
 *	Returns	: void
 */
void UpdateCamera()
{
	//	Retrieve focused objects zoom boundaries
	zoomBounds = focusedObject->GetMinMaxZoom();

	//	Set focus position to object position (world space)
	glm::vec3 focusPosition = focusedObject->transform->position;
	
	//	Camera focus transition speed is much faster on player to prevent judder
	float transitionAmount = 0.05f;
	if (focusedObject->tag == "player")
		transitionAmount = 0.95f;

	//	Check zoom is within boundaries and if not set to max bound
	if (zoom < zoomBounds.first)
	{
		zoom = zoomBounds.first;
	}
	else if (zoom > zoomBounds.second)
	{
		zoom = zoomBounds.second;
	}
	
	//	Determine camera and pivot ideal positions
	glm::vec3 targetPivot = focusPosition;
	glm::vec3 targetPosition = focusPosition - (mainCamera.GetViewDir() * zoom);

	//	Interpolate between current position and ideal position using transitionAmount 
	glm::vec4 currentPosition = glm::vec4(glm::mix(mainCamera.GetPosition(), targetPosition, transitionAmount), 1.0);
	glm::vec4 currentPivot = glm::vec4(glm::mix(mainCamera.GetLookAt(), targetPivot, transitionAmount), 1.0);
	
	//	Calculate Camera orientation using quaternions for camera yaw and pitch based on user mouse input
	float deltaX = (2 * PI / screenWidth); //Left -> right = 360
	float deltaY = (PI / screenHeight); //Top -> bottom = 180
	float yawIncrement = (lastMouse_x - mouse_x) * deltaX;
	float pitchIncrement = (lastMouse_y - mouse_y) * deltaY;
	glm::quat cameraYaw = glm::angleAxis(yawIncrement, mainCamera.GetUpVector());
	glm::quat cameraPitch = glm::angleAxis(pitchIncrement, mainCamera.GetRightVector());

	glm::quat cameraOrientation = cameraYaw * cameraPitch;
	
	//	Update camera position using the orientationMatrix and pivot positions
	currentPosition = (glm::toMat4(cameraOrientation) * (currentPosition - currentPivot)) + currentPivot;
	mainCamera.SetCameraView(currentPosition, currentPivot, glm::vec3(0.0, 1.0, 0.0));

	lastMouse_x = mouse_x;
	lastMouse_y = mouse_y;
}

/*
 *	Method	: DestroyPlayer
 *	Scope	: Private
 *	Purpose	: Handles player being destroyed
 *	Returns	: void
 */
void DestroyPlayer() 
{
	//	Set camera back to main camera 
	cockpitCam = false;
	//	Turn off player spotlight
	playerSpot.active = false;
	//	Set clear colour to indicate player destruction
	glClearColor(0.2, 0.0, 0.0, 0.8);	
}

/*
 *	Method	: UpdateOrbits
 *	Scope	: Private
 *	Purpose	: Updates all celestial body orbits
 *	Returns	: void
 */
void UpdateOrbits()
{
	//	Iterate over celestial bodies and update their orbits using deltaTime
	for (int i = 0; i < Bodies.size(); i++)
	{
		if (Bodies[i].orbitingBodyIndex != -1)
		{
			int orbitindex = Bodies[i].orbitingBodyIndex;
			Bodies[i].UpdateOrbit(Bodies[orbitindex].transform->position, deltaTime);
		}
	}
	//	Update satellite orbit position using deltaTime
	satellite.UpdateOrbit(Bodies[3].transform->position, deltaTime);
}

/*
 *	Method	: ApplyGravity
 *	Scope	: Private
 *	Purpose	: Applies gravity to applicable objects
 *	Returns	: void
 */
void ApplyGravity()
{
	//	Apply gravity to player object unless it's landed on celestial body
	if (!rocketShip.landed)
	{
		//	Store player position
		glm::vec3 playerPosition = rocketShip.transform->position;

		//	Calculate celestial body closest to the player
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
		//	Store closest planet position
		glm::vec3 planetPosition = Bodies[closestPlanetIndex].transform->position;

		//	Calculate 'attraction' directional vector
		glm::vec3 attractDirection = normalize(planetPosition - playerPosition);

		//	Calculate gravitational strength using object masses and distance
		float m1 = rocketShip.GetMass();
		float m2 = Bodies[closestPlanetIndex].GetMass();
		float strength = G * ((m1 * m2) / (nearest * nearest));

		//	Add gravitational force to player
		rocketShip.AddForce(attractDirection * strength);
	}
	
	//	If the satellite is not landed or in orbit then apply gravity
	if (!satellite.inOrbit && !satellite.landed)
	{
		//	Store satellite position
		glm::vec3 satellitePosition = satellite.transform->position;

		//	Calculate nearest celestial body to the craft
		float nearest = distance(Bodies[0].transform->position, satellitePosition);
		int closestPlanetIndex = 0;
		int currentPlanet = 0;
		for (auto it = Bodies.begin(); it != Bodies.end(); ++it)
		{
			float dist = distance(it->transform->position, satellitePosition);
			if (dist < nearest)
			{
				nearest = dist;
				closestPlanetIndex = currentPlanet;
			}
			++currentPlanet;
		}
		//	Store closest planet position
		glm::vec3 planetPosition = Bodies[closestPlanetIndex].transform->position;

		//	Calculate 'attraction' directional vector
		glm::vec3 attractDirection = normalize(planetPosition - satellitePosition);
		//	Calculate gravitational strength using object masses and distance
		float m1 = satellite.GetMass();
		float m2 = Bodies[closestPlanetIndex].GetMass();
		float strength = G * ((m1 * m2) / (nearest * nearest));

		//	Add gravitational force to satellite
		satellite.AddForce(attractDirection * strength);
	}
}

/*
 *	Method	: CheckCollisions
 *	Scope	: Private
 *	Purpose	: Checks collisions for applicable objects
 *	Returns	: void
 */
void CheckCollisions()
{
	//	Iterate over all celestial bodies
	for (auto it = Bodies.begin(); it != Bodies.end(); ++it)
	{
		//	If player is not landed check for collision
		if (!rocketShip.landed)
		{
			if (rocketShip.CheckCollision(*it))
			{
				//	If player collides set focused object to collision object and set throttle to zero
				accelerate = false;
				focusedObject = &(*it);
				//	If collision destroyed player, handle destruction
				if (rocketShip.destroyed)
				{
					DestroyPlayer();
				}
			}
		}
		
		//	If the satellite is not landed or in orbit then check for collision
		if (!satellite.inOrbit && !satellite.landed)
		{
			satellite.CheckCollision(*it);
		}
	}
	//	If the satellite is still in orbit check for collision with player
	if (satellite.inOrbit)
	{
		//Store ship velocity prior to collision check 
		glm::vec3 impactVelocity = rocketShip.GetVelocity();
		if (rocketShip.CheckCollision(satellite))
		{
			//	Same as above but we also inform the satellite of collision
			satellite.CollideWithPlayer(impactVelocity);
			accelerate = false;
			focusedObject = &rocketShip;
			if (rocketShip.destroyed)
			{
				DestroyPlayer();
			}
		}
	}
}

/*
 *	Method	: PlayerMovement
 *	Scope	: Private
 *	Purpose	: Handles player movement
 *	Returns	: void
 */
void PlayerMovement()
{	
	// If player is not landed or destroyed calculate rotation and movement
	if (!rocketShip.landed && !rocketShip.destroyed)
	{
		//	Determine individual rotational increments
		float yawInput = (Yaw * rocketShip.GetRotationSpeed()) * deltaTime;
		float pitchInput = (Pitch * rocketShip.GetRotationSpeed()) * deltaTime;
		float rollInput = (Roll * rocketShip.GetRotationSpeed()) * deltaTime;

		//	Pass these inputs to the player object which will convert them to quaternions
		rocketShip.transform->Rotate(pitchInput, yawInput, rollInput);

		//	Add forces based on forward and vertical thrust
		rocketShip.AddForce(rocketShip.transform->Forward() * (accelerate * rocketShip.GetSpeed()));
		rocketShip.AddForce(rocketShip.transform->Up() * (VerticalThrottle * 0.00008f));
	}
	//	If we're landed and thrusting vertically, take off
	else if (VerticalThrottle == 1.0f && !rocketShip.destroyed && rocketShip.landed)
	{
		rocketShip.TakeOff();
		focusedObject = &rocketShip;
	}
	//	Update rocket position based on timestep
	rocketShip.UpdatePosition(deltaTime);
}

/*
 *	Method	: PhysicsSimulation
 *	Scope	: Private
 *	Purpose	: Simulates physics
 *	Returns	: void
 */
void PhysicsSimulation() 
{
	//	Determine physics timestep based on frame time
	float currentTime = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = currentTime - lastFrameTime;
	lastFrameTime = currentTime;
	
	//	Call relevant physics related methods
	UpdateOrbits();
	ApplyGravity();
	PlayerMovement();
	CheckCollisions();
}

/*
 *	Method	: Special/SpecialUp
 *	Scope	: Private
 *	Purpose	: Handles 'special' key input
 *	Returns	: void
 */
void Special(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_SHIFT_L:
			accelerate = true;
			break;
	}
}
void SpecialUp(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_SHIFT_L:
			accelerate = false;
			break;
		//	Toggle cockpit camera view
		case GLUT_KEY_F1:
			cockpitCam = !cockpitCam;
			break;
		//	Toggle player collider drawing
		case GLUT_KEY_F2:
			showPlayerCollider = !showPlayerCollider;
			break;
		//	Toggle all collider drawing
		case GLUT_KEY_F3:
			showAllColliders = !showAllColliders;
			break;
	}
}

/*
 *	Method	: KeyDown/Keyup
 *	Scope	: Private
 *	Purpose	: Handles regular key input
 *	Returns	: void
 */
void KeyDown(unsigned char key, int x, int y)
{
	switch (std::tolower(key))
	{
		//	W A S D E Q keys are rotational input
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
		//	Space is vertical throttle input
		case ' ':
			VerticalThrottle = 1.0f;
			break;
	}
}
void KeyUp(unsigned char key, int x, int y)
{
	switch (std::tolower(key))
	{
	//	Reset inputs to zero on key up
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
		VerticalThrottle = 0.0f;
		break;
	//	If 'f' key pressed and player isn't destroyed, activate player spotlight
	case 'f':
		if (!rocketShip.destroyed)
			playerSpot.active = !playerSpot.active;
		break;
	//	If 'p' key pressed and player is destroyed, 'reset' player
	case 'p':
		if (rocketShip.destroyed)
		{
			accelerate = false;
			rocketShip.Reset(glm::vec3(120.0, 0.0, 20.0));
			focusedObject = &rocketShip;
			//	Set clear colour back to normal
			glClearColor(0.0, 0.0, 0.0, 0.0);
		}
		break;
	//	'.' and ',' change camera planet focus when player is landed
	case '.':
		if (rocketShip.landed)
		{
			if (++cameraIndex >= Bodies.size())
			{
				cameraIndex = 0;
			}
			focusedObject = &Bodies[cameraIndex];
		}
		break;
	case ',':
		if (rocketShip.landed)
		{
			if (--cameraIndex < 0)
			{
				cameraIndex = Bodies.size() -1;
			}
			focusedObject = &Bodies[cameraIndex];
		}
		break;
	}
}

/*
 *	Method	: UpdateMousePos/MouseInput
 *	Scope	: Private
 *	Purpose	: Handles mouse input
 *	Returns	: void
 */
void UpdateMousePos(int x, int y)
{
	//	Only track mouse x,y coordinates when middle is down
	if (MiddlePressed)
	{
		mouse_x = x;
		mouse_y = y;
	}
}
void MouseInput(int button, int state, int x, int y)
{
	//	Check for middle mouse click
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

/*
 *	Method	: ScrollWheel
 *	Scope	: Private
 *	Purpose	: Handles scroll input
 *	Returns	: void
 */
void ScrollWheel(int wheel, int dir, int x, int y)
{
	//	Set camera zoom amount based on currently set zoom boundaries
	dir = -dir;
	float zoomInc = zoomBounds.first * 0.5;
	if (zoom + (dir * zoomInc) > zoomBounds.first && zoom + (dir * zoomInc) < zoomBounds.second)
	{
		zoom += (dir * zoomInc);
	}
}

/*
 *	Method	: Idle
 *	Scope	: Private
 *	Purpose	: Calls all relevant main loop functions
 *	Returns	: void
 */
void Idle()
{
	//	First update physics
	PhysicsSimulation();
	//	Then update camera
	UpdateCamera();
	//	Finally request GLUT to render a new frame
	glutPostRedisplay();
}

#pragma endregion


/*
 *	Method	: Main
 *	Scope	: Private
 *	Purpose	: Initializes GLUT with all required callbacks
 *	Returns	: int (exit code)
 */
int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Space Simulation");

	//	This initialises glew - it must be called after the window is created.
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << " GLEW ERROR" << endl;
	}

	//	Check the OpenGL version being used
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
	cout << OpenGLVersion[0] << " " << OpenGLVersion[1] << endl;


	//	Initialise the simulation
	Init();

	//	Set reshape function callback
	glutReshapeFunc(Reshape);

	//	Set display function callback
	glutDisplayFunc(Display);

	//	Set keyboard input callbacks
	glutSpecialFunc(Special);
	glutSpecialUpFunc(SpecialUp);
	glutKeyboardFunc(KeyDown);
	glutKeyboardUpFunc(KeyUp);

	//	Set mouse input callbacks
	glutMotionFunc(UpdateMousePos);
	glutMouseFunc(MouseInput);
	glutMouseWheelFunc(ScrollWheel);
	glutIdleFunc(Idle);

	//starts the main loop. Program loops and calls callback functions as appropriate.
	glutMainLoop();

	return 0;
}
