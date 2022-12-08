#include "Transform.h"

Transform::Transform() : position(m_position), rotation(m_rotation), ModelMatrix(m_ModelMatrix)
{
	//Reset all transforms to identity
	m_position = glm::vec3(0.0);
	m_rotation = glm::quat(1.0, 0.0, 0.0, 0.0);
	m_ModelMatrix = glm::mat4(1.0);
}

void Transform::UpdateModelMatrix()
{
	m_ModelMatrix = glm::translate(glm::mat4(1.0), m_position);
	m_ModelMatrix *= glm::toMat4(m_rotation);
}

void Transform::Move(glm::vec3 newPosition)
{
	m_position = newPosition;
	UpdateModelMatrix();
}

void Transform::Move(glm::vec3 direction, float amount)
{
	m_position += direction * amount;
	UpdateModelMatrix();
}
void Transform::Rotate(float pitchIn, float yawIn, float rollIn)
{
	glm::quat pitch = glm::angleAxis(glm::radians(pitchIn), Side());
	glm::quat yaw = glm::angleAxis(glm::radians(yawIn), Up());
	glm::quat roll = glm::angleAxis(glm::radians(rollIn), Forward());
	m_rotation = (pitch * yaw * roll * m_rotation);
	UpdateModelMatrix();
}

void Transform::Reset()
{
	m_position = glm::vec3(0.0);
	m_rotation = glm::quat(1.0, 0.0, 0.0, 0.0);
	m_ModelMatrix = glm::mat4(1.0);
}