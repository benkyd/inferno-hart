#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace inferno {

class Camera {
public:
	Camera();
	Camera(int w, int h);

	void UpdateView();

	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetFrustrumMatrix();

	void UpdateProjection(int width, int height);

	// Keyboard
	void MoveCamera(glm::vec3 posDelta);
	// Mouse Delta
	void MouseMoved(glm::vec2 mouseDelta);

	// Updatable by 
	float MouseSensitivity = 0.1f;
	float CameraSpeed = 2.0f;

	void UpdatePosition(glm::vec3 position);
	void UpdateEulerLookDirection(float roll, float pitch, float yaw);
	void UpdateLookDirection(glm::vec3 lookDirection);

	glm::vec3 Position = {};
	float Roll, Pitch, Yaw;
	glm::vec3 LookDirection = {};

private:

	glm::mat4 viewMatrix = {};
	glm::mat4 projMatrix = {};
	
};


}


