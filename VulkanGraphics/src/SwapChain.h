#pragma once
#include "Device.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <string>
#include <vector>

class SwapChain {
public:
	SwapChain(Device& deviceRef, VkExtent2D windowExtent);
	SwapChain(Device& deviceRef, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous);
	~SwapChain();

	SwapChain(const SwapChain&) = delete;
	SwapChain& operator=(const SwapChain&) = delete;

	uint32_t width() { return swapChainExtent.width; }
	uint32_t height() { return swapChainExtent.height; }
	VkRenderPass getRenderPass() { return renderPass; }
	size_t imageCount() { return swapChainImages.size(); }
	VkExtent2D getSwapChainExtent() { return swapChainExtent; }
	VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
	VkImageView getImageView(int index) { return swapChainImageViews[index]; }
	VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
	float extentAspectRatio() { return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height); }

	VkFormat findDepthFormat();
	VkResult acquireNextImage(uint32_t* imageIndex);
	VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

	bool compareSwapFormats(const SwapChain& swapChain) const {
		return swapChain.swapChainDepthFormat == swapChainDepthFormat && swapChain.swapChainImageFormat == swapChainImageFormat;
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

	Device& device;
	size_t currentFrame = 0;
	VkExtent2D windowExtent;
	VkRenderPass renderPass;
	VkSwapchainKHR swapChain;
	VkExtent2D swapChainExtent;
	VkFormat swapChainImageFormat;
	VkFormat swapChainDepthFormat;
	std::vector<VkImage> depthImages;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> depthImageViews;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkDeviceMemory> depthImageMemorys;
	std::shared_ptr<SwapChain> m_PreviousSwapChain;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
};