#pragma once
#include <vector>
#include <string>

#include "Device.h"
#include "Model.h"

namespace Florencia {

	struct PipelineConfigInfo {
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class Pipeline {
	public:
		Pipeline(Device& device, const PipelineConfigInfo& info, const std::string& vertPath, const std::string& fragPath);
		~Pipeline();

		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		void Bind(VkCommandBuffer commandBuffer);

		static void DefaultPipelineConfigInfo(PipelineConfigInfo& info);
		static void EnableAlphaBlending(PipelineConfigInfo& info);

	private:
		static std::vector<char> ReadFile(const std::string& filepath);
		void CreateGraphicsPipeline(const PipelineConfigInfo& info, const std::string& vertPath, const std::string& fragPath);
		void CreateShaderModule(VkShaderModule* shaderModule, const std::vector<char>& code);

		Device& m_Device;
		VkPipeline m_GraphicsPipeline;
		VkShaderModule m_VertShaderModule, m_FragShaderModule;
	};

}
