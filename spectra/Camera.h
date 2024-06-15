#ifndef  CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = -15.0f;
const float SPEED = 7.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 100.0f;

class Camera
{
	public:	

		// camera Attributes
		glm::vec3 Position;
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;
		// euler Angles
		float Yaw;
		float Pitch;
		// camera options
		float MovementSpeed;
		float MouseSensitivity;
		float Zoom;
		float NearPlane;
		float FarPlane;
		// Screen dimensions
		unsigned int Width;
		unsigned int Height;

		// constructor with vectors
		Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : 
			Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), NearPlane(NEAR_PLANE), FarPlane(FAR_PLANE)
		{
			Position = position;
			WorldUp = up;
			Yaw = yaw;
			Pitch = pitch;
			updateCameraVectors();			
		}
		
		// constructor with scalar values
		Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : 
			Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), NearPlane(NEAR_PLANE), FarPlane(FAR_PLANE)
		{
			Position = glm::vec3(posX, posY, posZ);
			WorldUp = glm::vec3(upX, upY, upZ);
			Yaw = yaw;
			Pitch = pitch;
			updateCameraVectors();
		}
		
		glm::mat4 GetViewMatrix();
		void ProcessKeyboard(Camera_Movement direction, float deltaTime);
		void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
		void ProcessMouseScroll(float yoffset);

		void UpdateCameraMatrix(Shader& shader);
		void SetScreenDimensions(unsigned int width, unsigned int height);

		void FollowModel(glm::vec3& modPos, float dt);

	private:
		float lerpTime = 7.5f;
		void updateCameraVectors();

		glm::vec3 lerp(const glm::vec3& start, const glm::vec3& end, float dt) {
			return glm::mix(start, end, lerpTime * dt);
		}

};
#endif 