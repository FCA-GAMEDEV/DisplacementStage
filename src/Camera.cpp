#include "Camera.h"
#include <cmath>

Camera::Camera(void)
    : position(0.0f, 40.0f, 60.0f)
    , target(0.0f, 0.0f, 0.0f)
    , up(0.0f, 1.0f, 0.0f)
    , yaw(0.0f)
    , pitch(0.0f)
    , radius(1.0f)
    , fov(45.0f)
    , nearPlane(0.1f)
    , farPlane(1000.0f)
{
    float dx = target.x - position.x;
    float dy = target.y - position.y;
    float dz = target.z - position.z;
    radius = sqrt(dx*dx + dy*dy + dz*dz);
    if (radius < 0.001f) radius = 0.001f;
    pitch = glm::degrees(asin(dy / radius));
    yaw = glm::degrees(atan2(dz, dx));

    updateVectors();
}

Camera::~Camera(void)
{
}

glm::mat4 Camera::getViewMatrix(void) const
{
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectWidth, float aspectHeight) const
{
    float aspect = aspectWidth / aspectHeight;
    return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

void Camera::setPosition(float cx, float cy, float cz)
{
    position = glm::vec3(cx, cy, cz);
    float dx = target.x - position.x;
    float dy = target.y - position.y;
    float dz = target.z - position.z;
    radius = sqrt(dx*dx + dy*dy + dz*dz);
    if (radius < 0.001f) radius = 0.001f;
    pitch = glm::degrees(asin(dy / radius));
    yaw = glm::degrees(atan2(dz, dx));
    updateVectors();
}

void Camera::setPosition(const glm::vec3& pos)
{
    setPosition(pos.x, pos.y, pos.z);
}

glm::vec3 Camera::getPosition(void) const
{
    return position;
}

void Camera::setTarget(float tx, float ty, float tz)
{
    target = glm::vec3(tx, ty, tz);
    float dx = target.x - position.x;
    float dy = target.y - position.y;
    float dz = target.z - position.z;
    radius = sqrt(dx*dx + dy*dy + dz*dz);
    if (radius < 0.001f) radius = 0.001f;
    pitch = glm::degrees(asin(dy / radius));
    yaw = glm::degrees(atan2(dz, dx));
    updateVectors();
}

void Camera::setTarget(const glm::vec3& t)
{
    setTarget(t.x, t.y, t.z);
}

glm::vec3 Camera::getTarget(void) const
{
    return target;
}

float Camera::getYaw(void) const
{
    return yaw;
}

float Camera::getPitch(void) const
{
    return pitch;
}

void Camera::updateVectors(void)
{
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    forward = glm::normalize(f);

    right = glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f));
    if (glm::length(right) > 0.001f)
    {
        right = glm::normalize(right);
    }
    else
    {
        right = glm::vec3(1.0f, 0.0f, 0.0f);
    }
}

void Camera::updateCartesianFromSpherical(void)
{
    updateVectors();
    target = position + forward;
}

void Camera::move(float dx, float dy, float dz)
{
    updateVectors();
    position += right * dx + up * dy + forward * dz;
    updateCartesianFromSpherical();
}

void Camera::orbit(float dYaw, float dPitch)
{
    yaw += dYaw * 57.2957795f; 
    pitch += dPitch * 57.2957795f;

    if (pitch < -89.0f) pitch = -89.0f;
    if (pitch > 89.0f)  pitch = 89.0f;

    updateCartesianFromSpherical();
}

void Camera::zoom(float dRadius)
{
    move(0.0f, 0.0f, -dRadius);
}
