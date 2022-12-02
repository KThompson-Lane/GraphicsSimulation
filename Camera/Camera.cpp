#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 targetPosition, glm::vec3 upVector)
    : m_Position(std::move(position))
    , m_TargetPosition(std::move(targetPosition))
    , m_upVector(std::move(upVector))
{
    UpdateViewMatrix();
}

void Camera::SetCameraView(glm::vec3 position, glm::vec3 targetPosition, glm::vec3 up)
{
    m_Position = std::move(position);
    m_TargetPosition = std::move(targetPosition);
    m_upVector = std::move(up);
    UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
    // Generate view matrix using the eye, lookAt and up vector
    m_viewMatrix = glm::lookAt(m_Position, m_TargetPosition, m_upVector);
}