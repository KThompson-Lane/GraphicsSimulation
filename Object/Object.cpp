#include "Object.h"

void Object::init(char* modelFile)
{
	transform = new Transform();
	std::cout << " loading model " << std::endl;
	if (objectLoader.LoadModel(modelFile))//returns true if the model is loaded
	{
		std::cout << " model loaded " << std::endl;

		//copy data from the OBJLoader object to the threedmodel class
		model.ConstructModelFromOBJLoader(objectLoader);

		//if you want to translate the object to the origin of the screen,
		//first calculate the centre of the object, then move all the vertices
		//back so that the centre is on the origin.
		model.CalcCentrePoint();
		model.CentreOnZero();

		//Create bounding shader:
		boundingShader = CShader();
		if (!boundingShader.CreateShaderProgram("SimpleShader", "glslfiles/basic.vert", "glslfiles/basic.frag"))
		{
			std::cout << "failed to load shader" << std::endl;
		}
		model.InitVBO(&objectShader);
	}
	else
	{
		std::cout << " model failed to load " << std::endl;
	}
	m_Mass = model.CalcMass();
}

void Object::setupShader(char* shaderName, char* vertPath, char* fragPath)
{
	objectShader = CShader();
	if (!objectShader.CreateShaderProgram(shaderName, vertPath, fragPath))
	{
		std::cout << "failed to load shader" << std::endl;
	}
	glUseProgram(objectShader.GetProgramObjID());  // use the shader
	glEnable(GL_TEXTURE_2D);
}

//Begin Collider helper methods
void Object::AddSphereCollider()
{
	float radius = model.CalcBoundingSphere();
	collider = new SphereCollider(radius, this->transform);
	collider->CreateGeometry(boundingShader);
}

void Object::AddBoxCollider()
{
	float x, y, z;
	model.CalcOBB(x, y, z);
	glm::vec3 colliderSize = glm::vec3(x, y, z);
	collider = new BoxCollider(colliderSize, this->transform);
	collider->CreateGeometry(boundingShader);
}
//End Collider helper methods

void Object::render(glm::mat4& viewingMatrix, glm::mat4& ProjectionMatrix, bool showCollider, std::vector<PointLight>& lights, SpotLight& playerSpotLight)
{
	glUseProgram(objectShader.GetProgramObjID());  // use the shader
	//Quick fix for satellite model issue
	if (tag == "satellite")
	{
		glDisable(GL_CULL_FACE);
	}
	//Set deformation value
	glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), "deformation"), deformation.x, deformation.y, deformation.z);

	//Set the projection matrix in the shader
	GLuint projMatLocation = glGetUniformLocation(objectShader.GetProgramObjID(), "ProjectionMatrix");
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	//Set the view matrix in the shader
	glUniformMatrix4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	//Load all point lights into shader
	glm::vec3 lightEyePos;
	for (int i = 0; i < lights.size(); i++)
	{
		//TRANSFORM POSITION INTO EYESPACE
		lightEyePos = viewingMatrix * glm::vec4(lights[i].position, 1);

		//Construct light string and set values in shader
		std::string lightArrayString = "pointLights[";
		lightArrayString += std::to_string(i);
		lightArrayString += "]";
		glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), (lightArrayString+".position").c_str()), lightEyePos.x, lightEyePos.y, lightEyePos.z);

		glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), (lightArrayString + ".ambient").c_str()), lights[i].ambient.x, lights[i].ambient.y, lights[i].ambient.z);
		glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), (lightArrayString + ".diffuse").c_str()), lights[i].diffuse.x, lights[i].diffuse.y, lights[i].diffuse.z);
		glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), (lightArrayString + ".specular").c_str()), lights[i].specular.x, lights[i].specular.y, lights[i].specular.z);

		glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), (lightArrayString + ".constant").c_str()), lights[i].constant);
		glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), (lightArrayString + ".linear").c_str()), lights[i].linear);
		glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), (lightArrayString + ".quadratic").c_str()), lights[i].quadratic);
	}

	//Load player spot light into shader
	glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.position"), playerSpotLight.position.x, playerSpotLight.position.y, playerSpotLight.position.z);

	glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.ambient"), playerSpotLight.ambient.x, playerSpotLight.ambient.y, playerSpotLight.ambient.z);
	glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.diffuse"), playerSpotLight.diffuse.x, playerSpotLight.diffuse.y, playerSpotLight.diffuse.z);
	glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.specular"), playerSpotLight.specular.x, playerSpotLight.specular.y, playerSpotLight.specular.z);

	glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.constant"), playerSpotLight.constant);
	glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.linear"), playerSpotLight.linear);
	glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.quadratic"), playerSpotLight.quadratic);

	glUniform3f(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.direction"), playerSpotLight.direction.x, playerSpotLight.direction.y, playerSpotLight.direction.z);
	glUniform1f(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.cutOff"), glm::cos(glm::radians(playerSpotLight.cutOff)));

	glUniform1i(glGetUniformLocation(objectShader.GetProgramObjID(), "spotLight.enabled"), playerSpotLight.active);
	
	//Set the modelview matrix in the shader
	ModelViewMatrix = viewingMatrix * transform->ModelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(objectShader.GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	//Set the normal matrix in the shader
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(objectShader.GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	//Draw model
	model.DrawElementsUsingVBO(&objectShader);

	//Draw collider if shown
	if (showCollider)
	{
		collider->DrawCollider(boundingShader, ModelViewMatrix, ProjectionMatrix);
	}

	//Stop using our shader
	glUseProgram(0);
}

bool Object::CheckCollision(Object& other)
{
	return collider->InCollision(other.collider);
}
