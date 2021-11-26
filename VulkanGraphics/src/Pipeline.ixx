module;
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <stdexcept>
#include <vulkan/vulkan.h>
export module Pipeline;
export import Device;
export import Model;

export namespace Florencia {

	struct PipelineConfigInfo {
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

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

	private:
		static std::vector<char> ReadFile(const std::string& filepath);
		void CreateGraphicsPipeline(const PipelineConfigInfo& info, const std::string& vertPath, const std::string& fragPath);
		void CreateShaderModule(VkShaderModule* shaderModule, const std::vector<char>& code);

		Device& m_Device;
		VkPipeline m_GraphicsPipeline;
		VkShaderModule m_VertShaderModule, m_FragShaderModule;
	};

}

namespace Florencia {

	Pipeline::Pipeline(Device& device, const PipelineConfigInfo& info, const std::string& vertPath, const std::string& fragPath)
		:m_Device{ device } {
		CreateGraphicsPipeline(info, vertPath, fragPath);
	}

	Pipeline::~Pipeline() {
		vkDestroyShaderModule(m_Device.Get(), m_VertShaderModule, nullptr);
		vkDestroyShaderModule(m_Device.Get(), m_FragShaderModule, nullptr);
		vkDestroyPipeline(m_Device.Get(), m_GraphicsPipeline, nullptr);
	}

	void Pipeline::Bind(VkCommandBuffer commandBuffer) {
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
	}

	void Pipeline::DefaultPipelineConfigInfo(PipelineConfigInfo& info) {
		info.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		info.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		info.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		info.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		info.viewportInfo.viewportCount = 1;
		info.viewportInfo.pViewports = nullptr;
		info.viewportInfo.scissorCount = 1;
		info.viewportInfo.pScissors = nullptr;

		info.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		info.rasterizationInfo.depthClampEnable = VK_FALSE;
		info.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		info.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		info.rasterizationInfo.lineWidth = 1.0f;
		info.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		info.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		info.rasterizationInfo.depthBiasEnable = VK_FALSE;
		info.rasterizationInfo.depthBiasConstantFactor = 0.0f;
		info.rasterizationInfo.depthBiasClamp = 0.0f;
		info.rasterizationInfo.depthBiasSlopeFactor = 0.0f;

		info.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		info.multisampleInfo.sampleShadingEnable = VK_FALSE;
		info.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		info.multisampleInfo.minSampleShading = 1.0f;
		info.multisampleInfo.pSampleMask = nullptr;
		info.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
		info.multisampleInfo.alphaToOneEnable = VK_FALSE;

		info.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		info.colorBlendAttachment.blendEnable = VK_FALSE;
		info.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		info.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		info.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		info.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		info.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		info.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		info.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		info.colorBlendInfo.logicOpEnable = VK_FALSE;
		info.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
		info.colorBlendInfo.attachmentCount = 1;
		info.colorBlendInfo.pAttachments = &info.colorBlendAttachment;
		info.colorBlendInfo.blendConstants[0] = 0.0f;
		info.colorBlendInfo.blendConstants[1] = 0.0f;
		info.colorBlendInfo.blendConstants[2] = 0.0f;
		info.colorBlendInfo.blendConstants[3] = 0.0f;

		info.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		info.depthStencilInfo.depthTestEnable = VK_TRUE;
		info.depthStencilInfo.depthWriteEnable = VK_TRUE;
		info.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		info.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		info.depthStencilInfo.minDepthBounds = 0.0f;
		info.depthStencilInfo.maxDepthBounds = 1.0f;
		info.depthStencilInfo.stencilTestEnable = VK_FALSE;
		info.depthStencilInfo.front = {};
		info.depthStencilInfo.back = {};

		info.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		info.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		info.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(info.dynamicStateEnables.size());
		info.dynamicStateInfo.pDynamicStates = info.dynamicStateEnables.data();
		info.dynamicStateInfo.flags = 0;
	}

	std::vector<char> Pipeline::ReadFile(const std::string& filepath) {
		std::ifstream file{ filepath, std::ios::ate | std::ios::binary };
		if (!file.is_open()) {
			throw std::runtime_error("Failed to Open File: " + filepath);
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

	void Pipeline::CreateGraphicsPipeline(const PipelineConfigInfo& info, const std::string& vertPath, const std::string& fragPath) {
		assert(info.pipelineLayout != VK_NULL_HANDLE && "Cannot Create Graphics Pipeline: No Pipeline Layout Provided");
		assert(info.renderPass != VK_NULL_HANDLE && "Cannot Create Graphics Pipeline: No RenderPass Provided");

		auto vertCode = ReadFile(vertPath);
		auto fragCode = ReadFile(fragPath);

		CreateShaderModule(&m_VertShaderModule, vertCode);
		CreateShaderModule(&m_FragShaderModule, fragCode);

		VkPipelineShaderStageCreateInfo shaderStages[2];
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = m_VertShaderModule;
		shaderStages[0].pName = "main";
		shaderStages[0].flags = 0;
		shaderStages[0].pNext = nullptr;
		shaderStages[0].pSpecializationInfo = nullptr;

		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = m_FragShaderModule;
		shaderStages[1].pName = "main";
		shaderStages[1].flags = 0;
		shaderStages[1].pNext = nullptr;
		shaderStages[1].pSpecializationInfo = nullptr;

		auto bindingDescriptions = Model::Vertex::GetBindingDescriptions();
		auto attributeDescriptions = Model::Vertex::GetAttributeDescriptions();
		VkPipelineVertexInputStateCreateInfo vertInputInfo{};
		vertInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		vertInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

		VkGraphicsPipelineCreateInfo graphicsInfo{};
		graphicsInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsInfo.stageCount = 2;
		graphicsInfo.pStages = shaderStages;
		graphicsInfo.pVertexInputState = &vertInputInfo;
		graphicsInfo.pInputAssemblyState = &info.inputAssemblyInfo;
		graphicsInfo.pViewportState = &info.viewportInfo;
		graphicsInfo.pRasterizationState = &info.rasterizationInfo;
		graphicsInfo.pMultisampleState = &info.multisampleInfo;
		graphicsInfo.pColorBlendState = &info.colorBlendInfo;
		graphicsInfo.pDepthStencilState = &info.depthStencilInfo;
		graphicsInfo.pDynamicState = &info.dynamicStateInfo;
		graphicsInfo.flags = 0;

		graphicsInfo.pNext = nullptr;
		graphicsInfo.pTessellationState = nullptr;

		graphicsInfo.layout = info.pipelineLayout;
		graphicsInfo.renderPass = info.renderPass;
		graphicsInfo.subpass = info.subpass;

		graphicsInfo.basePipelineIndex = -1;
		graphicsInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(m_Device.Get(), VK_NULL_HANDLE, 1, &graphicsInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS) {
			throw std::runtime_error("Failed to Create Graphics Pipeline");
		}
	}

	void Pipeline::CreateShaderModule(VkShaderModule* shaderModule, const std::vector<char>& code) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		if (vkCreateShaderModule(m_Device.Get(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Failed to Create Shader Module");
		}
	}

}