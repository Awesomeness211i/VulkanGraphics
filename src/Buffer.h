#pragma once
#include "Device.h"

namespace Florencia {

	class Buffer {
	public:
		Buffer(Device& device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment = 1);
		~Buffer();

		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

		VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		void Unmap();

		void WriteToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkDescriptorBufferInfo DescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

		void WriteToIndex(void* data, int index);
		VkResult FlushIndex(int index);
		VkDescriptorBufferInfo DescriptorInfoForIndex(int index);
		VkResult InvalidateIndex(int index);

		VkBuffer GetBuffer() const { return m_Buffer; }
		void* GetMappedMemory() const { return m_Mapped; }
		uint32_t GetInstanceCount() const { return m_InstanceCount; }
		VkDeviceSize GetInstanceSize() const { return m_InstanceSize; }
		VkDeviceSize GetAlignmentSize() const { return m_AlignmentSize; }
		VkBufferUsageFlags GetUsageFlags() const { return m_UsageFlags; }
		VkMemoryPropertyFlags GetMemoryPropertyFlags() const { return m_MemoryPropertyFlags; }
		VkDeviceSize GetBufferSize() const { return m_BufferSize; }

	private:
		static VkDeviceSize GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

		Device& m_Device;
		void* m_Mapped = nullptr;
		uint32_t m_InstanceCount;
		VkDeviceSize m_BufferSize;
		VkDeviceSize m_InstanceSize;
		VkDeviceSize m_AlignmentSize;
		VkBufferUsageFlags m_UsageFlags;
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VkDeviceMemory m_Memory = VK_NULL_HANDLE;
		VkMemoryPropertyFlags m_MemoryPropertyFlags;
	};

}