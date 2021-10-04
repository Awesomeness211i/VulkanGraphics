module;
#include <stdexcept>

#include <vulkan/vulkan.h>
export module Buffer;
import Device;

export namespace Florencia {

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

module: private;

namespace Florencia {

	Buffer::Buffer(Device& device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment)
		: m_Device{ device }, m_InstanceSize{ instanceSize }, m_InstanceCount{ instanceCount },
		m_UsageFlags{ usageFlags }, m_MemoryPropertyFlags{ memoryPropertyFlags }
	{
		m_AlignmentSize = GetAlignment(instanceSize, minOffsetAlignment);
		m_BufferSize = m_AlignmentSize * instanceCount;
		device.createBuffer(m_BufferSize, usageFlags, memoryPropertyFlags, m_Buffer, m_Memory);
	}

	Buffer::~Buffer() {
		Unmap();
		vkDestroyBuffer(m_Device.device(), m_Buffer, nullptr);
		vkFreeMemory(m_Device.device(), m_Memory, nullptr);
	}

	VkResult Buffer::Map(VkDeviceSize size, VkDeviceSize offset) {
		if (!m_Buffer || !m_Memory) { throw std::runtime_error("Called map on buffer before create"); }
		return vkMapMemory(m_Device.device(), m_Memory, offset, size, 0, &m_Mapped);
	}

	void Buffer::Unmap() {
		if (m_Mapped) {
			vkUnmapMemory(m_Device.device(), m_Memory);
			m_Mapped = nullptr;
		}
	}

	void Buffer::WriteToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset) {
		if (!m_Mapped) { throw std::runtime_error("Cannot copy to unmapped buffer"); }
		if (size == VK_WHOLE_SIZE) {
			memcpy(m_Mapped, data, m_BufferSize);
		}
		else {
			char* memOffset = (char*)m_Mapped;
			memOffset += offset;
			memcpy(memOffset, data, size);
		}
	}

	VkResult Buffer::Flush(VkDeviceSize size, VkDeviceSize offset) {
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = m_Memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkFlushMappedMemoryRanges(m_Device.device(), 1, &mappedRange);
	}

	VkDescriptorBufferInfo Buffer::DescriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
		return VkDescriptorBufferInfo{ m_Buffer, offset, size, };
	}

	VkResult Buffer::Invalidate(VkDeviceSize size, VkDeviceSize offset) {
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = m_Memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkInvalidateMappedMemoryRanges(m_Device.device(), 1, &mappedRange);
	}

	void Buffer::WriteToIndex(void* data, int index) {
		WriteToBuffer(data, m_InstanceSize, index * m_AlignmentSize);
	}

	VkResult Buffer::FlushIndex(int index) {
		return Flush(m_AlignmentSize, index * m_AlignmentSize);
	}

	VkDescriptorBufferInfo Buffer::DescriptorInfoForIndex(int index) {
		return DescriptorInfo(m_AlignmentSize, index * m_AlignmentSize);
	}

	VkResult Buffer::InvalidateIndex(int index) {
		return Invalidate(m_AlignmentSize, index * m_AlignmentSize);
	}

	VkDeviceSize Buffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
		if (minOffsetAlignment > 0) {
			return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
		}
		return instanceSize;
	}

}