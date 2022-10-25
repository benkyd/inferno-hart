#include <scene/camera.hpp>

using namespace inferno;

Camera::Camera()
{
	projMatrix = glm::perspective( glm::radians( 45.0f ), 1.0f, 0.1f, 1000.0f );

	Roll = 0.0f;
	Pitch = 0.0f;
	Yaw = 0.0f;

	Position = {};
	LookDirection = {};

	viewMatrix = {};

	UpdateView();
}

Camera::Camera(int w, int h)
{
	projMatrix = glm::perspective(glm::radians(45.0f), (float)w / (float)h, 0.1f, 1000.0f);

	Roll = 0.0f;
	Pitch = 0.0f;
	Yaw = 0.0f;

	Position = {};
	LookDirection = {};

	viewMatrix = {};

	UpdateView();
}

void Camera::UpdateView()
{
	// roll can be removed
	glm::mat4 matRoll = glm::mat4(1.0f); //identity matrix; 
	glm::mat4 matPitch = glm::mat4(1.0f);//identity matrix
	glm::mat4 matYaw = glm::mat4(1.0f);  //identity matrix

	// roll, pitch and yaw
	matRoll = glm::rotate( matRoll, Roll, glm::vec3(0.0f, 0.0f, 1.0f));
	matPitch = glm::rotate( matPitch, Pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	matYaw = glm::rotate( matYaw, Yaw, glm::vec3( 0.0f, 1.0f, 0.0f));

	glm::mat4 rotate = matRoll * matPitch * matYaw;

	glm::mat4 translate = glm::mat4(1.0f);
	translate = glm::translate(translate, -Position);

	viewMatrix = rotate * translate;

	// Work out Look Vector
	glm::mat4 inverseView = glm::inverse(viewMatrix);

	LookDirection.x = inverseView[2][0];
	LookDirection.y = inverseView[2][1];
	LookDirection.z = inverseView[2][2];
}

glm::mat4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix()
{
	return projMatrix;
}

void Camera::UpdateProjection(int width, int height)
{
	projMatrix = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
}

void Camera::MoveCamera(glm::vec3 posDelta)
{
	// Rotate by camera direction
	glm::mat2 rotate {
		cos(Yaw), -sin(Yaw),
		sin(Yaw), cos(Yaw)
	};

	glm::vec2 f(0.0, 1.0);
	f = f * rotate;

	// get current view matrix
	glm::mat4 mat = GetViewMatrix();
	glm::vec3 forward(mat[0][2], mat[1][2], mat[2][2]);
	glm::vec3 strafe(mat[0][0], mat[1][0], mat[2][0]);

	// forward vector must be negative to look forward. 
	// read :http://in2gpu.com/2015/05/17/view-matrix/
	Position.x += posDelta.x * CameraSpeed;
	Position.z += posDelta.z * CameraSpeed;
	Position.y += posDelta.y * CameraSpeed;

	// update the view matrix
	UpdateView();
}

void Camera::MouseMoved(glm::vec2 mouseDelta)
{
	// note that yaw and pitch must be converted to radians.
	// this is done in UpdateView() by glm::rotate
	Yaw += MouseSensitivity * (mouseDelta.x / 100);
	Pitch += MouseSensitivity * (mouseDelta.y / 100);
	Pitch = glm::clamp<float>(Pitch, -M_PI / 2, M_PI / 2);

	UpdateView();
}

void Camera::UpdatePosition(glm::vec3 position)
{
	Position = position;

	UpdateView();
}

void Camera::UpdateEulerLookDirection(float roll, float pitch, float yaw)
{
	Roll = roll; Pitch = pitch; Yaw = yaw;
	LookDirection.x = cos(Yaw) * cos(Pitch);
	LookDirection.y = sin(Yaw) * cos(Pitch);
	LookDirection.z = sin(Pitch);

	UpdateView();
}

void Camera::UpdateLookDirection(glm::vec3 lookDirection)
{
	LookDirection = lookDirection;
	Pitch = asin(-lookDirection.y);
	Yaw = atan2(lookDirection.x, lookDirection.z);

	UpdateView();
}
