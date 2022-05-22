#pragma once
#include <memory>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>
#include "Model.h"

namespace Florencia {

	struct TransformComponent {
		glm::vec3 translation{ 0.0f };
		glm::vec3 scale{ 1.0f };
		glm::vec3 rotation{ 0.0f };

		//Matrix multiplication of "Translation * RotationX * RotationZ * RotationY * Scale"
		//Rotations are Tait-bryan angles of "Y(1), X(2), Z(3)"
		glm::mat4 Mat4();
		glm::mat3 NormalMatrix();
	};

	struct PointLightComponent {
		float m_LightIntensity = 1.0f;
	};

	class GameObject {
	public:
		using ID_t = unsigned int;
		using Map_t = std::unordered_map<ID_t, GameObject>;

		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;

		static GameObject CreateGameObject() {
			static ID_t currentID = 0;
			return GameObject{ currentID++ };
		}

		static GameObject CreatePointLight(float intensity = 10.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));

		const ID_t GetID() { return m_ID; }

		glm::vec4 m_Color{};
		TransformComponent m_Transform;

		//optional components
		std::shared_ptr<Model> m_Model{};
		std::unique_ptr<PointLightComponent> m_PointLight = nullptr;

	private:
		GameObject(ID_t objId) : m_ID{ objId } {}
		ID_t m_ID;
	};

}