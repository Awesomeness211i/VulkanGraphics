#pragma once
#include "GameObject.h"
#include "Camera.h"

#define MAX_LIGHTS 10

namespace Florencia {

	struct PointLight {
		glm::vec4 m_Position{};
		glm::vec4 m_Color{};
	};

	struct GlobalUBO {
		glm::mat4 m_ProjectionMatrix{1.0f};
		glm::mat4 m_ViewMatrix{1.0f};
		glm::mat4 m_InverseViewMatrix{1.0f};
		glm::vec4 m_AmbientLightColor{0.0f, 0.0f, 0.0f, 0.0f}; //4th component is light intensity
		PointLight m_PointLights[MAX_LIGHTS];
		int numLights;
	};

	struct FrameInfo {
		Camera& m_Camera;
		int m_FrameIndex;
		float m_FrameTime;
		VkCommandBuffer m_CommandBuffer;
		VkDescriptorSet m_GlobalDescriptorSet;
		GameObject::Map_t& m_GameObjects;
	};

}