#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Direction {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// default values
const float YAW        =  0.0f;
const float PITCH      =  0.0f;
const float SPEED      =  4.0f;
const float SENSITIVTY =  0.1f;
const float FOV        =  45.0f;

class Camera {

public:
    float yaw;         // angle of rotation about y axis
    float pitch;       // angle of rotation about x axis
    float speed;       // movement speed
    float sensitivity; // mouse sensitivity
    float fov;         // field of view angle in degrees
    glm::vec3 position;
    glm::vec3 world_up;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;    

    Camera(glm::vec3 position, glm::vec3 up, float init_yaw, float init_pitch);
    
    glm::mat4 get_view();
    
    void process_mouse_movement(float xoffset, float yoffset);
    
    void process_mouse_scroll(float yoffset);
    
    void process_keyboard_input(Camera_Direction direction, float dt);

private:

    void update_camera_vectors();
};

#endif 

Camera::Camera(
    glm::vec3 position,
    glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f),
    float init_yaw = YAW,
    float init_pitch = PITCH
    ):
    position(position),
    world_up(world_up),
    yaw(init_yaw),
    pitch(init_pitch),
    speed(SPEED),
    sensitivity(SENSITIVTY),
    fov(FOV)
{
    update_camera_vectors();
}

glm::mat4 Camera::get_view()
{
    glm::vec3 target = position + front;
    return glm::lookAt(position, target, world_up);
}

void Camera::process_mouse_movement(float xoffset, float yoffset)
{
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    yaw   += xoffset;
    pitch += yoffset;
    if (pitch > 89.0f) {
	pitch = 89.0f;
    }
    if (pitch < -89.0f) {
	pitch = -89.0f;
    }
    update_camera_vectors();
}

void Camera::process_mouse_scroll(float yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f) {
	fov -= yoffset;
    } else if (fov <= 1.0f) {
	fov = 1.0f;
    } else if (fov >= 45.0f) {
	fov = 45.0f;
    }
}

void Camera::process_keyboard_input(Camera_Direction direction, float dt)
{
    float move_speed = speed * dt;
    if (direction == FORWARD)
	position += glm::normalize(glm::vec3(front.x, 0.0f, front.z)) * move_speed;
    if (direction == BACKWARD)
	position -= glm::normalize(glm::vec3(front.x, 0.0f, front.z)) * move_speed;
    if (direction == LEFT)
	position -= right * move_speed;
    if (direction == RIGHT)
	position += right * move_speed;
    if (direction == UP)
	position += world_up * move_speed;
    if (direction == DOWN)
	position -= world_up * move_speed;
}

void Camera::update_camera_vectors()
{
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);
    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));
}
    
