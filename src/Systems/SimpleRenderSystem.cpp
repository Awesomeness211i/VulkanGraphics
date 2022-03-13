#include "SimpleRenderSystem.h"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>
#include <stdexcept>

#include "GameObject.h"

namespace Florencia {

	struct SimplePushConstantData {
		glm::mat4 modelMatrix{ 1.0f };
		glm::mat4 normalMatrix{ 1.0f };
	};

	SimpleRenderSystem::SimpleRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : m_Device(device) {
		CreatePipelineLayout(globalSetLayout);
		CreatePipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem() { vkDestroyPipelineLayout(m_Device.Get(), m_PipelineLayout, nullptr); }

	void SimpleRenderSystem::RenderGameObjects(FrameInfo& frameInfo) {
		m_Pipeline->Bind(frameInfo.m_CommandBuffer);

		vkCmdBindDescriptorSets(frameInfo.m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &frameInfo.m_GlobalDescriptorSet, 0, nullptr);

		for (auto& keyvalue : frameInfo.m_GameObjects) {
			auto& obj = keyvalue.second;
			if (obj.m_Model == nullptr) { continue; }
			SimplePushConstantData push{};
			push.modelMatrix = obj.m_Transform.Mat4();
			push.normalMatrix = obj.m_Transform.NormalMatrix();

			vkCmdPushConstants(frameInfo.m_CommandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			obj.m_Model->Bind(frameInfo.m_CommandBuffer);
			obj.m_Model->Draw(frameInfo.m_CommandBuffer);
		}
	}

	void SimpleRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout) {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.size = sizeof(SimplePushConstantData);
		pushConstantRange.offset = 0;

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(m_Device.Get(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) throw std::runtime_error("Failed to Create Pipeline Layout");
	}

	void SimpleRenderSystem::CreatePipeline(VkRenderPass renderPass) {
		if (m_PipelineLayout == nullptr) throw std::runtime_error("Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		Pipeline::DefaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_PipelineLayout;

		m_Pipeline = std::make_unique<Pipeline>(m_Device, pipelineConfig, "assets/shaders/World.vert.spv", "assets/shaders/World.frag.spv");
	}

}
