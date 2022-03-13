#pragma once
#include <memory>
#include <vector>
#include "FrameInfo.h"
#include "Pipeline.h"
#include "Device.h"

namespace Florencia {

	class PointLightSystem {
	public:
		PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void Update(FrameInfo& frameInfo, GlobalUBO& ubo);
		void Render(FrameInfo& frameInfo);
	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void CreatePipeline(VkRenderPass renderPass);

		Device& m_Device;
		VkPipelineLayout m_PipelineLayout;
		std::unique_ptr<Pipeline> m_Pipeline;
	};

}
