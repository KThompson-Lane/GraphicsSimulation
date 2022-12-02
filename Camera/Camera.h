#pragma once
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
class Camera
{
private:
    glm::mat4 m_viewMatrix;
    glm::vec3 m_eye; // Camera position in 3D
    glm::vec3 m_lookAt; // Point that the camera is looking at
    glm::vec3 m_upVector; // Orientation of the camera
public:
private:
public:
    Camera() = default;
    Camera(glm::vec3 eye, glm::vec3 lookat, glm::vec3 upVector);
    glm::mat4 GetViewMatrix();
    glm::vec3 GetEye();
    glm::vec3 GetUpVector();
    glm::vec3 GetLookAt();
    glm::vec3 GetViewDir();
    glm::vec3 GetRightVector();
    void SetCameraView(glm::vec3 eye, glm::vec3 lookat, glm::vec3 up);
    void UpdateViewMatrix();
};

