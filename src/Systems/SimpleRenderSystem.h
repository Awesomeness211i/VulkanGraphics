#pragma once
#include <memory>
#include <vector>
#include "FrameInfo.h"
#include "Pipeline.h"
#include "Device.h"

namespace Florencia {

	class SimpleRenderSystem {
	public:
		SimpleRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void RenderGameObjects(FrameInfo& frameInfo);
	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void CreatePipeline(VkRenderPass renderPass);

		Device& m_Device;
		VkPipelineLayout m_PipelineLayout;
		std::unique_ptr<Pipeline> m_Pipeline;
	};

}