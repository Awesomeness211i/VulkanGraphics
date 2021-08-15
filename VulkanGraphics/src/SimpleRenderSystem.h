#pragma once
#include "Camera.h"
#include "Device.h"
#include "Pipeline.h"
#include "GameObject.h"

#include <memory>
#include <vector>

class SimpleRenderSystem {
public:
	SimpleRenderSystem(Device& device, VkRenderPass renderPass);
	~SimpleRenderSystem();

	SimpleRenderSystem(const SimpleRenderSystem&) = delete;
	SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

	void RenderGameObjects(VkCommandBuffer cmdBuffer, std::vector<GameObject>& gameObjects, const Camera& camera);
private:
	void CreatePipelineLayout();
	void CreatePipeline(VkRenderPass renderPass);

	Device& m_Device;
	VkPipelineLayout m_PipelineLayout;
	std::unique_ptr<Pipeline> m_Pipeline;
};