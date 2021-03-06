#include "Camera.h"
#include <stdexcept>
#include <limits>

namespace Florencia {

	void Camera::SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far) {
		m_ProjectionMatrix = glm::mat4{ 1.0f };
		m_ProjectionMatrix[0][0] = 2.0f / (right - left);
		m_ProjectionMatrix[1][1] = 2.0f / (bottom - top);
		m_ProjectionMatrix[2][2] = 1.0f / (far - near);
		m_ProjectionMatrix[3][0] = -(right + left) / (right - left);
		m_ProjectionMatrix[3][1] = -(bottom + top) / (bottom - top);
		m_ProjectionMatrix[3][2] = -near / (far - near);
	}

	void Camera::SetPerspectiveProjection(float fovy, float aspect, float near, float far) {
		if (glm::abs(aspect - std::numeric_limits<float>::epsilon()) < 0.0f) { throw std::runtime_error("Out of Bounds"); }
		const float tanHalfFOVY = tan(fovy / 2.0f);
		m_ProjectionMatrix = glm::mat4{ 0.0f };
		m_ProjectionMatrix[0][0] = 1.0f / (aspect * tanHalfFOVY);
		m_ProjectionMatrix[1][1] = 1.0f / tanHalfFOVY;
		m_ProjectionMatrix[2][2] = far / (far - near);
		m_ProjectionMatrix[2][3] = 1.0f;
		m_ProjectionMatrix[3][2] = -(far * near) / (far - near);
	}

	void Camera::SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
		const glm::vec3 w{ glm::normalize(direction) };
		const glm::vec3 u{ glm::normalize(glm::cross(w, up)) };
		const glm::vec3 v{ glm::cross(w, u) };

		m_ViewMatrix = glm::mat4{ 1.0f };
		m_ViewMatrix[0][0] = u.x;
		m_ViewMatrix[1][0] = u.y;
		m_ViewMatrix[2][0] = u.z;

		m_ViewMatrix[0][1] = v.x;
		m_ViewMatrix[1][1] = v.y;
		m_ViewMatrix[2][1] = v.z;

		m_ViewMatrix[0][2] = w.x;
		m_ViewMatrix[1][2] = w.y;
		m_ViewMatrix[2][2] = w.z;

		m_ViewMatrix[3][0] = -glm::dot(u, position);
		m_ViewMatrix[3][1] = -glm::dot(v, position);
		m_ViewMatrix[3][2] = -glm::dot(w, position);

		m_InverseViewMatrix = glm::mat4{1.0f};
		m_InverseViewMatrix[0][0] = u.x;
		m_InverseViewMatrix[0][1] = u.y;
		m_InverseViewMatrix[0][2] = u.z;

		m_InverseViewMatrix[1][0] = v.x;
		m_InverseViewMatrix[1][1] = v.y;
		m_InverseViewMatrix[1][2] = v.z;

		m_InverseViewMatrix[2][0] = w.x;
		m_InverseViewMatrix[2][1] = w.y;
		m_InverseViewMatrix[2][2] = w.z;

		m_InverseViewMatrix[3][0] = position.x;
		m_InverseViewMatrix[3][1] = position.y;
		m_InverseViewMatrix[3][2] = position.z;
	}

	void Camera::SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
		SetViewDirection(position, target - position, up);
	}

	void Camera::SetViewYXZ(glm::vec3 position, glm::vec3 rotation) {
		//Rotations are Tait-bryan angles of "Y(1), X(2), Z(3)"
		const float cz = glm::cos(rotation.z);
		const float sz = glm::sin(rotation.z);
		const float cx = glm::cos(rotation.x);
		const float sx = glm::sin(rotation.x);
		const float cy = glm::cos(rotation.y);
		const float sy = glm::sin(rotation.y);

		const glm::vec3 u{ (cy * cz + sy * sx * sz), (cx * sz), (cy * sx * sz - cz * sy) };
		const glm::vec3 v{ (cz * sy * sx - cy * sz), (cx * cz), (cy * cz * sx + sy * sz) };
		const glm::vec3 w{ (cx * sy), (-sx), (cy * cx) };

		m_ViewMatrix = glm::mat4{1.0f};
		m_ViewMatrix[0][0] = u.x;
		m_ViewMatrix[1][0] = u.y;
		m_ViewMatrix[2][0] = u.z;

		m_ViewMatrix[0][1] = v.x;
		m_ViewMatrix[1][1] = v.y;
		m_ViewMatrix[2][1] = v.z;

		m_ViewMatrix[0][2] = w.x;
		m_ViewMatrix[1][2] = w.y;
		m_ViewMatrix[2][2] = w.z;

		m_ViewMatrix[3][0] = -glm::dot(u, position);
		m_ViewMatrix[3][1] = -glm::dot(v, position);
		m_ViewMatrix[3][2] = -glm::dot(w, position);

		m_InverseViewMatrix = glm::mat4{1.0f};
		m_InverseViewMatrix[0][0] = u.x;
		m_InverseViewMatrix[0][1] = u.y;
		m_InverseViewMatrix[0][2] = u.z;

		m_InverseViewMatrix[1][0] = v.x;
		m_InverseViewMatrix[1][1] = v.y;
		m_InverseViewMatrix[1][2] = v.z;

		m_InverseViewMatrix[2][0] = w.x;
		m_InverseViewMatrix[2][1] = w.y;
		m_InverseViewMatrix[2][2] = w.z;

		m_InverseViewMatrix[3][0] = position.x;
		m_InverseViewMatrix[3][1] = position.y;
		m_InverseViewMatrix[3][2] = position.z;
	}

}