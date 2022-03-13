#pragma once
#include <memory>
#include "SwapChain.h"
#include "Window.h"
#include "Device.h"

namespace Florencia {

	class Renderer {
	public:
		Renderer(Window& window, Device& device);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		VkRenderPass GetSwapChainRenderPass() const { return m_SwapChain->getRenderPass(); }
		bool IsFrameInProgress() const { return m_FrameStarted; }

		float GetAspectRatio() const { return m_SwapChain->extentAspectRatio(); }

		VkCommandBuffer GetCurrentCommandBuffer() const {
			if (!m_FrameStarted) throw std::runtime_error("Cannot Get Command Buffer When Frame Not Started");
			return m_CommandBuffers[m_CurrentFrameIndex];
		}

		int GetFrameIndex() const {
			if (!m_FrameStarted) throw std::runtime_error("Cannot Get Frame Index When Frame Not Started");
			return m_CurrentFrameIndex;
		}

		VkCommandBuffer BeginFrame();
		void EndFrame();

		void BeginSwapChainRenderPass(VkCommandBuffer buffer);
		void EndSwapChainRenderPass(VkCommandBuffer buffer);
	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapchain();

		Window& m_Window;
		Device& m_Device;
		std::unique_ptr<SwapChain> m_SwapChain;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		bool m_FrameStarted{ false };
		int m_CurrentFrameIndex{ 0 };
		uint32_t m_CurrentImageIndex;
	};

}
