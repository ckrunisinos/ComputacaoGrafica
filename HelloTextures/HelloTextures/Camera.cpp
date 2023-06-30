#include "Camera.h"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

void Camera::initialize(float initialX, float initialY, float initialZ)
{
    this->x = initialX;
    this->y = initialY;
    this->z = initialZ;

    this->speed = 0.08;

    this->cameraFront = glm::vec3(0.0, 0.0, -1.0);
    this->cameraUp = glm::vec3(0.0, 1.0, 0.0);

    this->firstMouse = true;
    this->isCrouching = false;
    this -> sensitivity = 0.05;
    this->pitch = 0.0;
    this->yaw = -90.0;
}

glm::vec3 Camera::getPosition() {
    return glm::vec3(x, y, z);
}

glm::mat4 Camera::getView() {
    glm::vec3 cameraPosition = getPosition();
    return glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
}

void Camera::toggleCrouch() {
    if (isCrouching) {
        standUp();
    } else {
        crouch();
    }
}

void Camera::crouch() {
    y = -0.2;
    isCrouching = true;
}

void Camera::standUp() {
    y = 0.0;
    isCrouching = false;
}

void Camera::moveLeft() {
    glm::vec3 newPosition = glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
    x -= newPosition.x;
    y -= newPosition.y;
    z -= newPosition.z;
}

void Camera::moveRight() {
    glm::vec3 newPosition = glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
    x += newPosition.x;
    y += newPosition.y;
    z += newPosition.z;
}

void Camera::moveForward() {
    z -= speed;
}

void Camera::moveBackward() {
    z += speed;
}

void Camera::mouse_callback(double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float offsetx = xpos - lastX;
	float offsety = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	offsetx *= sensitivity;
	offsety *= sensitivity;

	pitch += offsety;
	yaw += offsetx;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}