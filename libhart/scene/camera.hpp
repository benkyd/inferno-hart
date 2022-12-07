#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <mutex>

namespace inferno {

class Camera {
public:
	Camera();
	Camera(int w, int h);

	void update();
	bool didUpdate();
	void newFrame();

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
	glm::mat4 getCameraLook();

	void setRasterViewport(glm::vec2 viewport);

	// Keyboard
	void moveCamera(uint8_t posDelta);
	// Mouse Delta
	void mouseMoved(glm::vec2 mouseDelta);

	void setPosition(glm::vec3 position);
	void setEulerLook(float roll, float pitch, float yaw);
	void setLook(glm::vec3 lookDirection);

public:
	void setRayViewport(glm::vec2 viewport);
	glm::vec2 getRayViewport();

public:
	// necessary evil
	float MouseSensitivity = 0.4f;
	float CameraSpeed = 0.1f;
	float Roll, Pitch, Yaw;
	float FOV = 45.0f;
	glm::vec3 Position = {};
	glm::vec3 LookDirection = {};

private:
	glm::vec2 mViewport = { 100.0f, 100.0f };
	glm::vec2 mRayViewport = { 300.0f, 300.0f };
	glm::mat4 mViewMatrix = {};
	glm::mat4 mProjMatrix = {};
	glm::mat4 mCameraLook = {};
	bool mDidUpdate;

	std::mutex _mCam;
};

}
