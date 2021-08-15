#pragma once
#include "Window.h"

#include <string>
#include <vector>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
bool presentFamilyHasValue = false;
bool graphicsFamilyHasValue = false;
uint32_t graphicsFamily, presentFamily;
bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
};

class Device {
public:
    Device(Window& window);
    ~Device();

    // Not copyable or movable
    Device(Device&&) = delete;
    Device(const Device&) = delete;
    Device& operator=(Device&&) = delete;
    Device& operator=(const Device&) = delete;

    VkDevice device() { return device_; }
    VkSurfaceKHR surface() { return surface_; }
    VkQueue presentQueue() { return presentQueue_; }
    VkQueue graphicsQueue() { return graphicsQueue_; }
    VkCommandPool getCommandPool() { return commandPool; }

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice); }
    SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    // Buffer Helper Functions
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);
    void createImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    VkPhysicalDeviceProperties properties;
    #ifdef NDEBUG
    const bool enableValidationLayers = false;
    #else
    const bool enableValidationLayers = true;
    #endif
private:
    void createSurface();
    void createInstance();
    void createCommandPool();
    void pickPhysicalDevice();
    void setupDebugMessenger();
    void createLogicalDevice();
    
    // helper functions
    bool checkValidationLayerSupport();
    void hasGflwRequiredInstanceExtensions();
    bool isDeviceSuitable(VkPhysicalDevice device);
    std::vector<const char*> getRequiredExtensions();
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    Window& window;
    VkInstance instance;
    VkCommandPool commandPool;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    
    VkDevice device_;
    VkSurfaceKHR surface_;
    VkQueue graphicsQueue_;
    VkQueue presentQueue_;

    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
    const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
};