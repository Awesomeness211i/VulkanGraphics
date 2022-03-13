#pragma once
#include <vector>

#include "Window.h"

namespace Florencia {

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR m_Capabilities;
        std::vector<VkSurfaceFormatKHR> m_Formats;
        std::vector<VkPresentModeKHR> m_PresentModes;
    };

    struct QueueFamilyIndices {
        bool m_PresentFamilyHasValue = false;
        bool m_GraphicsFamilyHasValue = false;
        uint32_t m_GraphicsFamily, m_PresentFamily;
        bool IsComplete() { return m_GraphicsFamilyHasValue && m_PresentFamilyHasValue; }
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

        VkDevice Get() { return m_Device; }
        VkSurfaceKHR GetSurface() { return m_Surface; }
        VkQueue PresentQueue() { return m_PresentQueue; }
        VkQueue GraphicsQueue() { return m_GraphicsQueue; }
        VkCommandPool GetCommandPool() { return m_CommandPool; }

        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        QueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(m_PhysicalDevice); }
        SwapChainSupportDetails GetSwapChainSupport() { return QuerySwapChainSupport(m_PhysicalDevice); }
        VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        // Buffer Helper Functions
        VkCommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);
        void CreateImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

        VkPhysicalDeviceProperties properties;
        bool m_EnableValidationLayers = true;
    private:
        void CreateSurface();
        void CreateInstance();
        void CreateCommandPool();
        void PickPhysicalDevice();
        void SetupDebugMessenger();
        void CreateLogicalDevice();

        // helper functions
        bool CheckValidationLayerSupport();
        void HasGflwRequiredInstanceExtensions();
        bool IsDeviceSuitable(VkPhysicalDevice device);
        std::vector<const char*> GetRequiredExtensions();
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

        Window& m_Window;
        VkInstance m_Instance;
        VkCommandPool m_CommandPool;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

        VkDevice m_Device;
        VkSurfaceKHR m_Surface;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;

        const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    };

}
