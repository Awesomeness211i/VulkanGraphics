#include "PointLightSystem.h"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>
#include <stdexcept>
#include <map>

#include "GameObject.h"

namespace Florencia {

	struct PointLightPushConstants {
		glm::vec4 m_Position{};
		glm::vec4 m_Color{};
		float radius;
	};

	PointLightSystem::PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : m_Device(device) {
		CreatePipelineLayout(globalSetLayout);
		CreatePipeline(renderPass);
	}

	PointLightSystem::~PointLightSystem() { vkDestroyPipelineLayout(m_Device.Get(), m_PipelineLayout, nullptr); }

	void PointLightSystem::Update(FrameInfo& frameInfo, GlobalUBO& ubo) {
		int lightIndex = 0;
		for(auto& kv : frameInfo.m_GameObjects) {
			auto& obj = kv.second;
			if(obj.m_PointLight == nullptr) { continue; }
			ubo.m_PointLights[lightIndex].m_Position = glm::vec4(obj.m_Transform.translation, 1.0f);
			ubo.m_PointLights[lightIndex].m_Color = glm::vec4(obj.m_Color.r, obj.m_Color.g, obj.m_Color.b, obj.m_PointLight->m_LightIntensity);
			lightIndex++;
		}
		ubo.numLights = lightIndex;
	}

	void PointLightSystem::Render(FrameInfo& frameInfo) {
		//sort lights
		std::map<float, GameObject::ID_t> m_SortedLights;
		for(auto& kv : frameInfo.m_GameObjects) {
			auto& obj = kv.second;
			if(obj.m_PointLight == nullptr) { continue; }

			//calculate distance
			auto offset = frameInfo.m_Camera.GetPostition() - obj.m_Transform.translation;
			float squareDistance = glm::dot(offset, offset);
			m_SortedLights[squareDistance] = obj.GetID();
		}

		m_Pipeline->Bind(frameInfo.m_CommandBuffer);

		vkCmdBindDescriptorSets(frameInfo.m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &frameInfo.m_GlobalDescriptorSet, 0, nullptr);

		for(auto it = m_SortedLights.rbegin(); it != m_SortedLights.rend(); ++it) {
			auto& obj = frameInfo.m_GameObjects.at(it->second);
			PointLightPushConstants push{};
			push.m_Position = glm::vec4(obj.m_Transform.translation, 1.0f);
			push.m_Color = glm::vec4(obj.m_Color.x, obj.m_Color.y, obj.m_Color.z, obj.m_PointLight->m_LightIntensity);
			push.radius = obj.m_Transform.scale.x;

			vkCmdPushConstants(
				frameInfo.m_CommandBuffer,
				m_PipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(PointLightPushConstants),
				&push
			);

			vkCmdDraw(frameInfo.m_CommandBuffer, 6, 1, 0, 0);
		}
	}

	void PointLightSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout) {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.size = sizeof(PointLightPushConstants);
		pushConstantRange.offset = 0;

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(m_Device.Get(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) { throw std::runtime_error("Failed to Create Pipeline Layout"); }
	}

	void PointLightSystem::CreatePipeline(VkRenderPass renderPass) {
		if (m_PipelineLayout == nullptr) { throw std::runtime_error("Cannot create pipeline before pipeline layout"); }

		PipelineConfigInfo pipelineConfig{};
		Pipeline::DefaultPipelineConfigInfo(pipelineConfig);
		Pipeline::EnableAlphaBlending(pipelineConfig);
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.bindingDescriptions.clear();

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_PipelineLayout;

		m_Pipeline = std::make_unique<Pipeline>(m_Device, pipelineConfig, "assets/shaders/PointLight.vert.spv", "assets/shaders/PointLight.frag.spv");
	}

}