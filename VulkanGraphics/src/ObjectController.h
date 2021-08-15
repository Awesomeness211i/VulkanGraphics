#pragma once

#include "Window.h"
#include "GameObject.h"

//Potentially Temporary
class ObjectController {
public:
	enum class KeyMappings {
		MoveLeft = GLFW_KEY_A,
		MoveRight = GLFW_KEY_D,
		MoveForeward = GLFW_KEY_W,
		MoveBackward = GLFW_KEY_S,
		MoveUp = GLFW_KEY_SPACE,
		MoveDown = GLFW_KEY_LEFT_SHIFT, 
		LookLeft = GLFW_KEY_LEFT,
		LookRight = GLFW_KEY_RIGHT,
		LookUp = GLFW_KEY_UP,
		LookDown = GLFW_KEY_DOWN
	};

	void MoveInPlaneXZ(GLFWwindow* window, float timestep, GameObject& object);

private:
	float m_MoveSpeed{ 3.0f }, m_LookSpeed{1.5f};
};