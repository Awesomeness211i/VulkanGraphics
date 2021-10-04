module;
#include <memory>
#include <vector>
#include <stdexcept>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <vulkan/vulkan.h>
export module SimpleRenderSystem;
import GameObject;
import FrameInfo;
import Pipeline;
import Device;

export namespace Florencia {

	class SimpleRenderSystem {
	public:
		SimpleRenderSystem(Device& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void RenderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects);
	private:
		void CreatePipelineLayout();
		void CreatePipeline(VkRenderPass renderPass);

		Device& m_Device;
		VkPipelineLayout m_PipelineLayout;
		std::unique_ptr<Pipeline> m_Pipeline;
	};

}

module: private;

namespace Florencia {

	struct SimplePushConstantData {
		glm::mat4 transform{ 1.0f };
		glm::mat4 normalMatrix{ 1.0f };
	};

	SimpleRenderSystem::SimpleRenderSystem(Device& device, VkRenderPass renderPass) : m_Device(device) {
		CreatePipelineLayout();
		CreatePipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem() { vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr); }

	void SimpleRenderSystem::RenderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects) {
		m_Pipeline->Bind(frameInfo.CommandBuffer);

		auto viewProjectionMatrix = frameInfo.Camera.GetProjectionMatrix() * frameInfo.Camera.GetViewMatrix();

		for (auto& obj : gameObjects) {
			SimplePushConstantData push{};
			auto modelMatrix = obj.m_Transform.Mat4();
			push.transform = viewProjectionMatrix * modelMatrix;
			push.normalMatrix = obj.m_Transform.NormalMatrix();

			vkCmdPushConstants(frameInfo.CommandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			obj.m_Model->Bind(frameInfo.CommandBuffer);
			obj.m_Model->Draw(frameInfo.CommandBuffer);
		}
	}

	void SimpleRenderSystem::CreatePipelineLayout() {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.size = sizeof(SimplePushConstantData);
		pushConstantRange.offset = 0;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) throw std::runtime_error("Failed to Create Pipeline Layout");
	}

	void SimpleRenderSystem::CreatePipeline(VkRenderPass renderPass) {
		if (m_PipelineLayout == nullptr) throw std::runtime_error("Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		Pipeline::DefaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_PipelineLayout;

		m_Pipeline = std::make_unique<Pipeline>(m_Device, pipelineConfig, "assets/shaders/shader.vert.spv", "assets/shaders/shader.frag.spv");
	}

}