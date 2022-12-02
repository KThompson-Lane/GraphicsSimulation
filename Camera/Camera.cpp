#include "Camera.h"

Camera::Camera(glm::vec3 eye, glm::vec3 lookat, glm::vec3 upVector)
    : m_eye(std::move(eye))
    , m_lookAt(std::move(lookat))
    , m_upVector(std::move(upVector))
{
    UpdateViewMatrix();
}

glm::mat4 Camera::GetViewMatrix() { return m_viewMatrix; }
glm::vec3 Camera::GetEye() { return m_eye; }
glm::vec3 Camera::GetUpVector() { return m_upVector; }
glm::vec3 Camera::GetLookAt() { return m_lookAt; }

glm::vec3 Camera::GetViewDir() { return -glm::transpose(m_viewMatrix)[2]; }
glm::vec3 Camera::GetRightVector() { return glm::transpose(m_viewMatrix)[0]; }

void Camera::SetCameraView(glm::vec3 eye, glm::vec3 lookat, glm::vec3 up)
{
    m_eye = std::move(eye);
    m_lookAt = std::move(lookat);
    m_upVector = std::move(up);
    UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
    // Generate view matrix using the eye, lookAt and up vector
    m_viewMatrix = glm::lookAt(m_eye, m_lookAt, m_upVector);
}