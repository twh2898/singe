#include "s3e/Camera.hpp"
#include "s3e/Util.hpp"
#include "s3e/log.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Tom::s3e {

    Camera::Camera(): rotation(0, 0), position(0, 0, 0), projectionMode(Perspective), fov(45.0f) { }

    Camera::~Camera() { }

    void Camera::setFov(float fov) {
        this->fov = fov;
    }

    float Camera::getFov() {
        return fov;
    }

    void Camera::setScreenSize(sf::Vector2u screen) {
        this->screenSize = screen;
    }

    Camera::ProjectionMode Camera::getProjection() {
        return projectionMode;
    }

    void Camera::setProjection(Camera::ProjectionMode mode) {
        projectionMode = mode;
    }

    const glm::vec2 & Camera::getRotation() const {
        return rotation;
    }

    void Camera::setRotation(glm::vec2 rot) {
        this->rotation = rot;
    }

    void Camera::rotate(glm::vec2 delta) {
        rotation -= delta;
        rotation.x = glm::clamp(rotation.x, -89.0f, 89.0f);
    }

    const glm::vec3 & Camera::getPosition() const {
        return position;
    }

    void Camera::setPosition(glm::vec3 pos) {
        this->position = pos;
    }

    void Camera::move(glm::vec3 delta) {
        position += delta;
    }

    void Camera::moveDolly(glm::vec3 delta) {
        float yRot = glm::radians(rotation.y);

        float dz = -delta.x * std::sin(yRot) + delta.z * std::cos(yRot);
        float dx = -delta.x * -std::cos(yRot) + delta.z * std::sin(yRot);

        float dy = delta.y;

        position += glm::vec3(dx, dy, dz);
    }

    glm::mat4 Camera::projMatrix() {
        float r = (float)screenSize.x / (float)screenSize.y;
        if (projectionMode == Perspective)
            return glm::perspective(glm::radians(fov), r, 0.1f, 100.0f);
        else
            return glm::ortho<float>(0, screenSize.x, screenSize.y, 0);
    }

    glm::mat4 Camera::viewMatrix() {
        glm::mat4 m (1);
        m = glm::rotate(m, -glm::radians(rotation.x), glm::vec3(1, 0, 0));
        m = glm::rotate(m, -glm::radians(rotation.y), glm::vec3(0, 1, 0));
        m = glm::translate(m, -position);
        return m;
    }
}

