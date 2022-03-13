#include "GameObject.h"

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

	GameObject GameObject::CreatePointLight(float intensity, float radius, glm::vec3 color) {
		GameObject pointLight = GameObject::CreateGameObject();
		pointLight.m_Color = {color.r, color.g, color.b, 0.0};
		pointLight.m_Transform.scale.x = radius;
		pointLight.m_PointLight = std::make_unique<PointLightComponent>();
		pointLight.m_PointLight->m_LightIntensity = intensity;
		return pointLight;
	}

}
