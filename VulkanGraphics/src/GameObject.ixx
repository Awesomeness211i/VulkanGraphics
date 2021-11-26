module;
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
export module GameObject;
import Model;

export namespace Florencia {

	struct TransformComponent {
		glm::vec3 translation{ 0.0f };
		glm::vec3 scale{ 1.0f };
		glm::vec3 rotation{ 0.0f };

		//Matrix multiplication of "Translation * RotationX * RotationZ * RotationY * Scale"
		//Rotations are Tait-bryan angles of "Y(1), X(2), Z(3)"
		glm::mat4 Mat4();
		glm::mat3 NormalMatrix();
	};

	class GameObject {
	public:
		using ID_t = unsigned int;

		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;

		static GameObject CreateGameObject() {
			static ID_t currentID = 0;
			return GameObject{ currentID++ };
		}
		const ID_t GetID() { return m_ID; }

		TransformComponent m_Transform;
		std::shared_ptr<Model> m_Model{};
		glm::vec4 m_Color{};

	private:
		GameObject(ID_t objId) : m_ID{ objId } {}
		ID_t m_ID;
	};

}

namespace Florencia {

	glm::mat4 TransformComponent::Mat4() {
		const float cz = glm::cos(rotation.z);
		const float sz = glm::sin(rotation.z);
		const float cx = glm::cos(rotation.x);
		const float sx = glm::sin(rotation.x);
		const float cy = glm::cos(rotation.y);
		const float sy = glm::sin(rotation.y);
		return {
			{ scale.x * (cy * cz + sy * sx * sz), scale.x * (cx * sz), scale.x * (cy * sx * sz - cz * sy), 0.0f },
			{ scale.y * (cz * sy * sx - cy * sz), scale.y * (cx * cz), scale.y * (cy * cz * sx + sy * sz), 0.0f },
			{ scale.z * (cx * sy), scale.z * (-sx), scale.z * (cy * cx), 0.0f },
			{ translation.x, translation.y, translation.z, 1.0f }
		};
	}

	glm::mat3 TransformComponent::NormalMatrix() {
		const float cz = glm::cos(rotation.z);
		const float sz = glm::sin(rotation.z);
		const float cx = glm::cos(rotation.x);
		const float sx = glm::sin(rotation.x);
		const float cy = glm::cos(rotation.y);
		const float sy = glm::sin(rotation.y);
		const glm::vec3 inverseScale = 1.0f / scale;
		return {
			{ inverseScale.x * (cy * cz + sy * sx * sz), inverseScale.x * (cx * sz), inverseScale.x * (cy * sx * sz - cz * sy) },
			{ inverseScale.y * (cz * sy * sx - cy * sz), inverseScale.y * (cx * cz), inverseScale.y * (cy * cz * sx + sy * sz) },
			{ inverseScale.z * (cx * sy), inverseScale.z * (-sx), inverseScale.z * (cy * cx) }
		};
	}

}