#pragma once

#include <glm/glm.hpp>

class Camera
{
    public:
        Camera() {}
        ~Camera() {}
        void initialize(float initialX, float initialY, float initialZ);
        glm::vec3 getPosition();
        glm::mat4 getView();

        // movement
        void moveLeft();
        void moveRight();
        void moveForward();
        void moveBackward();

        void toggleCrouch();

        void mouse_callback(double xpos, double ypos);

    protected:
        float speed;
        float x, y, z;
        glm::vec3 cameraUp;
        glm::vec3 cameraFront;
        bool isCrouching;
        bool firstMouse;
        float lastX, lastY;
        float sensitivity, pitch, yaw;

        void crouch();
        void standUp();
};
