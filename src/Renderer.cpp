#include "Renderer.h"
#include <stdexcept>
#include <array>

namespace Florencia {

	Renderer::Renderer(Window& window, Device& device) :m_Window(window), m_Device(device) {
		RecreateSwapchain();
		CreateCommandBuffers();
	}

	Renderer::~Renderer() { FreeCommandBuffers(); }

	VkCommandBuffer Renderer::BeginFrame() {
		if (m_FrameStarted) throw std::runtime_error("Cannot Begin A Frame While One Is Already Started");
		VkResult result = m_SwapChain->acquireNextImage(&m_CurrentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			RecreateSwapchain();
			return nullptr;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) throw std::runtime_error("Failed to Aquire Next SwapChain Image");

		m_FrameStarted = true;

		VkCommandBufferBeginInfo beginInfo{};
		VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) throw std::runtime_error("Failed to Begin Recording Command Buffer");
		return commandBuffer;
	}

	void Renderer::EndFrame() {
		if (!m_FrameStarted) throw std::runtime_error("Cannot End A Frame While One Is Not Started");
		VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) throw std::runtime_error("Failed to Record Command Buffer");

		VkResult result = m_SwapChain->submitCommandBuffers(&commandBuffer, &m_CurrentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.WasResized()) {
			m_Window.ResetWindowResizeFlag();
			RecreateSwapchain();
		}
		else if (result != VK_SUCCESS) throw std::runtime_error("Failed to Present SwapChain Image");

		m_FrameStarted = false;
		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void Renderer::BeginSwapChainRenderPass(VkCommandBuffer buffer) {
		if (!m_FrameStarted) throw std::runtime_error("Can't Call BeginSwapChainRenderPass If No Frame Is Started");
		if (buffer != GetCurrentCommandBuffer()) throw std::runtime_error("Can't Call BeginSwapChainRenderPass On Command Buffer From Different Frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_SwapChain->getRenderPass();
		renderPassInfo.framebuffer = m_SwapChain->getFrameBuffer(m_CurrentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_SwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(m_SwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, m_SwapChain->getSwapChainExtent() };
		vkCmdSetViewport(buffer, 0, 1, &viewport);
		vkCmdSetScissor(buffer, 0, 1, &scissor);
	}

	void Renderer::EndSwapChainRenderPass(VkCommandBuffer buffer) {
		if (!m_FrameStarted) throw std::runtime_error("Can't Call EndSwapChainRenderPass If No Frame Is Started");
		if (buffer != GetCurrentCommandBuffer()) throw std::runtime_error("Can't Call EndSwapChainRenderPass On Command Buffer From Different Frame");

		vkCmdEndRenderPass(buffer);
	}

	void Renderer::CreateCommandBuffers() {
		m_CommandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandPool = m_Device.GetCommandPool();
		allocateInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(m_Device.Get(), &allocateInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to Allocate Command Buffer");
		}
	}

	void Renderer::FreeCommandBuffers() {
		vkFreeCommandBuffers(m_Device.Get(), m_Device.GetCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
		m_CommandBuffers.clear();
	}

	void Renderer::RecreateSwapchain() {
		auto extent = m_Window.GetExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = m_Window.GetExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(m_Device.Get());
		if (m_SwapChain == nullptr) m_SwapChain = std::make_unique<SwapChain>(m_Device, extent);
		else {
			std::shared_ptr<SwapChain> oldSwapChain = std::move(m_SwapChain);
			m_SwapChain = std::make_unique<SwapChain>(m_Device, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*m_SwapChain.get())) throw std::runtime_error("Swap chain image(or depth) format has changed!");
		}
	}

}
