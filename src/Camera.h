#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    Camera(void);
    ~Camera(void);

    // Getters de Matrizes
    glm::mat4 getViewMatrix(void) const;
    glm::mat4 getProjectionMatrix(float aspectWidth, float aspectHeight) const;

    // Setters e Getters de Estado
    void setPosition(float cx, float cy, float cz);
    void setPosition(const glm::vec3& pos);
    glm::vec3 getPosition(void) const;

    void setTarget(float tx, float ty, float tz);
    void setTarget(const glm::vec3& target);
    glm::vec3 getTarget(void) const;

    float getYaw(void) const;
    float getPitch(void) const;

    // Controles de Navegação (Movimento Livre e Orbital)
    void move(float dx, float dy, float dz);
    void orbit(float dYaw, float dPitch);
    void zoom(float dRadius);

private:
    void updateCartesianFromSpherical(void);
    void updateVectors(void);

    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    
    // Vetores de direção local
    glm::vec3 forward;
    glm::vec3 right;

    // Coordenadas esféricas para órbita/direção
    float yaw;
    float pitch;
    float radius;
    
    float fov; // Campo de visão
    float nearPlane;
    float farPlane;
};
