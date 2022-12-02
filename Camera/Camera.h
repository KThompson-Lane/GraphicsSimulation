#pragma once
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
class Camera
{
private:
    glm::mat4 m_viewMatrix;
    glm::vec3 m_Position; // Camera position in 3D
    glm::vec3 m_TargetPosition; // Point that the camera is looking at
    glm::vec3 m_upVector; // Orientation of the camera
public:
private:
public:
    Camera() = default;
    Camera(glm::vec3 position, glm::vec3 targetPosition, glm::vec3 upVector);
    void SetCameraView(glm::vec3 position, glm::vec3 targetPosition, glm::vec3 up);
    void UpdateViewMatrix();

    glm::mat4 GetViewMatrix() const { return m_viewMatrix; }
    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetUpVector() const { return m_upVector; }
    glm::vec3 GetLookAt() const { return m_TargetPosition; }

    glm::vec3 GetViewDir() const { return -glm::transpose(m_viewMatrix)[2]; }
    glm::vec3 GetRightVector() const { return glm::transpose(m_viewMatrix)[0]; }
};

