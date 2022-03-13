#pragma once
#include <memory>
#include <vector>

#include "Device.h"

namespace Florencia {

	class SwapChain {
	public:
		SwapChain(Device& deviceRef, VkExtent2D m_WindowExtent);
		SwapChain(Device& deviceRef, VkExtent2D m_WindowExtent, std::shared_ptr<SwapChain> previous);
		~SwapChain();

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

		uint32_t width() { return m_SwapChainExtent.width; }
		uint32_t height() { return m_SwapChainExtent.height; }
		VkRenderPass getRenderPass() { return m_RenderPass; }
		size_t imageCount() { return m_SwapChainImages.size(); }
		VkExtent2D getSwapChainExtent() { return m_SwapChainExtent; }
		VkFormat getSwapChainImageFormat() { return m_SwapChainImageFormat; }
		VkImageView getImageView(int index) { return m_SwapChainImageViews[index]; }
		VkFramebuffer getFrameBuffer(int index) { return m_SwapChainFramebuffers[index]; }
		float extentAspectRatio() { return static_cast<float>(m_SwapChainExtent.width) / static_cast<float>(m_SwapChainExtent.height); }

		VkFormat findDepthFormat();
		VkResult acquireNextImage(uint32_t* imageIndex);
		VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

		bool compareSwapFormats(const SwapChain& m_SwapChain) const {
			return m_SwapChain.m_SwapChainDepthFormat == m_SwapChainDepthFormat && m_SwapChain.m_SwapChainImageFormat == m_SwapChainImageFormat;
		}

		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
	private:
		void Init();
		void createSwapChain();
		void createImageViews();
		void createRenderPass();
		void createSyncObjects();
		void createFramebuffers();
		void createDepthResources();

		// Helper functions
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		Device& m_Device;
		size_t m_CurrentFrame = 0;
		VkExtent2D m_WindowExtent;
		VkRenderPass m_RenderPass;
		VkSwapchainKHR m_SwapChain;
		VkExtent2D m_SwapChainExtent;
		VkFormat m_SwapChainImageFormat;
		VkFormat m_SwapChainDepthFormat;
		std::vector<VkImage> m_DepthImages;
		std::vector<VkFence> m_InFlightFences;
		std::vector<VkFence> m_ImagesInFlight;
		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_DepthImageViews;
		std::vector<VkImageView> m_SwapChainImageViews;
		std::vector<VkDeviceMemory> m_DepthImageMemorys;
		std::shared_ptr<SwapChain> m_PreviousSwapChain;
		std::vector<VkFramebuffer> m_SwapChainFramebuffers;
		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
	};

}
