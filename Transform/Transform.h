#pragma once
#include "../glm/glm.hpp"
//Angry 4thDimensional spinning noises
#include "../glm/gtc/quaternion.hpp"
#include "../glm/gtx/quaternion.hpp"

class Transform {
//private members
private:
	glm::vec3 m_position;
	glm::quat m_rotation;
	glm::mat4 m_ModelMatrix;

//public members
public: 
	const glm::vec3& position;
	const glm::quat& rotation;
	const glm::mat4& ModelMatrix;

//private methods
private:
	void UpdateModelMatrix();
//public methods
public:
	Transform();
	const glm::vec3 Side() { return glm::normalize(glm::vec3(m_ModelMatrix[0][0], m_ModelMatrix[0][1], m_ModelMatrix[0][2])); }
	const glm::vec3 Up() { return glm::normalize(glm::vec3(m_ModelMatrix[1][0], m_ModelMatrix[1][1], m_ModelMatrix[1][2])); }
	const glm::vec3 Forward() { return glm::normalize(glm::vec3(m_ModelMatrix[2][0], m_ModelMatrix[2][1], m_ModelMatrix[2][2])); }
	void Move(glm::vec3 newPosition);
	void Move(glm::vec3 direction, float amount);
	void Rotate(float pitchIn, float yawIn, float rollIn);
	void Reset();
};