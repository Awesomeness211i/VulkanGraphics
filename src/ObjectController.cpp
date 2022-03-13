#include "ObjectController.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <limits>


namespace Florencia {

	void ObjectController::MoveInPlaneXZ(GLFWwindow* window, float timestep, GameObject& object) {
		glm::vec3 rotate{ 0 };
		if (glfwGetKey(window, (int)KeyMappings::LookRight) == GLFW_PRESS) { rotate.y += 1.0f; }
		if (glfwGetKey(window, (int)KeyMappings::LookLeft) == GLFW_PRESS) { rotate.y -= 1.0f; }

		if (glfwGetKey(window, (int)KeyMappings::LookUp) == GLFW_PRESS) { rotate.x += 1.0f; }
		if (glfwGetKey(window, (int)KeyMappings::LookDown) == GLFW_PRESS) { rotate.x -= 1.0f; }

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) { object.m_Transform.rotation += m_LookSpeed * timestep * glm::normalize(rotate); }
		object.m_Transform.rotation.x = glm::clamp(object.m_Transform.rotation.x, -1.5f, 1.5f);
		object.m_Transform.rotation.y = glm::mod(object.m_Transform.rotation.y, 2 * glm::pi<float>());

		const glm::vec3 forward{ sin(object.m_Transform.rotation.y), 0.0f, cos(object.m_Transform.rotation.y) },
			right{ forward.z, 0.0f, -forward.x },
			up{ 0.0f, -1.0f, 0.0f };

		glm::vec3 moveDir{ 0.0f };
		if (glfwGetKey(window, (int)KeyMappings::MoveForeward) == GLFW_PRESS) { moveDir += forward; }
		if (glfwGetKey(window, (int)KeyMappings::MoveBackward) == GLFW_PRESS) { moveDir -= forward; }
		if (glfwGetKey(window, (int)KeyMappings::MoveRight) == GLFW_PRESS) { moveDir += right; }
		if (glfwGetKey(window, (int)KeyMappings::MoveLeft) == GLFW_PRESS) { moveDir -= right; }
		if (glfwGetKey(window, (int)KeyMappings::MoveUp) == GLFW_PRESS) { moveDir += up; }
		if (glfwGetKey(window, (int)KeyMappings::MoveDown) == GLFW_PRESS) { moveDir -= up; }

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) { object.m_Transform.translation += m_MoveSpeed * timestep * glm::normalize(moveDir); }
	}

}
