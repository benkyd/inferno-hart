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
	void UpdateProjection();

	// Keyboard
	void MoveCamera(uint8_t posDelta);
	// Mouse Delta
	void MouseMoved(glm::vec2 mouseDelta);

	// Updatable by 
	float MouseSensitivity = 0.4f;
	float CameraSpeed = 0.1f;

	void UpdatePosition(glm::vec3 position);
	void UpdateEulerLookDirection(float roll, float pitch, float yaw);
	void UpdateLookDirection(glm::vec3 lookDirection);

	glm::vec3 Position = {};
	float Roll, Pitch, Yaw;
	float FOV = 45.0f;
	glm::vec3 LookDirection = {};

private:
	glm::vec2 mViewport = {};
	glm::mat4 mViewMatrix = {};
	glm::mat4 mProjMatrix = {};
	
};

}
