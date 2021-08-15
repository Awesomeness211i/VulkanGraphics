#pragma once
#include <memory>
#include <glm/gtc/matrix_transform.hpp>

#include "Model.h"


struct TransformComponent {
	glm::vec3 translation{0.0f};
	glm::vec3 scale{1.0f};
	glm::vec3 rotation{0.0f};

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
		return GameObject{currentID++};
	}
	const ID_t GetID() { return m_ID; }

	TransformComponent m_Transform;
	std::shared_ptr<Model> m_Model{};
	glm::vec4 m_Color{};

private:
	GameObject(ID_t objId) : m_ID{objId} {}
	ID_t m_ID;
};