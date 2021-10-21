module;
#include <cassert>
#include <stdexcept>

#include <memory>
#include <vector>
#include <unordered_map>

#include <vulkan/vulkan.h>
export module Descriptors;
import Device;

export namespace Florencia {

	class DescriptorSetLayout {
	public:
		DescriptorSetLayout(Device& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
		~DescriptorSetLayout();

		DescriptorSetLayout(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

		VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }

		class Builder {
		public:
			Builder(Device& device) : m_Device{ device } {}

			Builder& AddBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count = 1);
			std::unique_ptr<DescriptorSetLayout> Build() const;
		private:
			Device& m_Device;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings{};
		};
	private:
		Device& m_Device;
		VkDescriptorSetLayout m_DescriptorSetLayout;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings;
		friend class DescriptorWriter;
	};
	
	class DescriptorPool {
	public:
		DescriptorPool(Device& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes);
		~DescriptorPool();

		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool& operator=(const DescriptorPool&) = delete;

		bool AllocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;
		void FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;
		void ResetPool();
		
		class Builder {
		public:
			Builder(Device& device) : m_Device{ device } {}

			Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
			Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& SetMaxSets(uint32_t count);

			std::unique_ptr<DescriptorPool> Build() const;
		private:
			Device& m_Device;
			std::vector<VkDescriptorPoolSize> m_PoolSizes{};
			uint32_t m_MaxSets = 1000;
			VkDescriptorPoolCreateFlags m_PoolFlags = 0;
		};
	private:
		Device& m_Device;
		VkDescriptorPool m_DescriptorPool;
		friend class DescriptorWriter;
	};

	class DescriptorWriter {
	public:
		DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool);

		DescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		DescriptorWriter& WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

		bool Build(VkDescriptorSet& set);
		void Overwrite(VkDescriptorSet& set);
	private:
		DescriptorPool& m_Pool;
		DescriptorSetLayout& m_SetLayout;
		std::vector<VkWriteDescriptorSet> m_Writes;
	};

}

module: private;

namespace Florencia {

	//DescriptorSetLayout Builder
	DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count) {
		assert(m_Bindings.count(binding) == 0 && "Binding already in use");
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stageFlags;
		m_Bindings[binding] = layoutBinding;
		return *this;
	}

	std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() const {
		return std::make_unique<DescriptorSetLayout>(m_Device, m_Bindings);
	}

	//DescriptorPool Builder
	DescriptorPool::Builder& DescriptorPool::Builder::AddPoolSize(VkDescriptorType descriptorType, uint32_t count) {
		m_PoolSizes.push_back({ descriptorType, count });
		return *this;
	}

	DescriptorPool::Builder& DescriptorPool::Builder::SetPoolFlags(VkDescriptorPoolCreateFlags flags) {
		m_PoolFlags = flags;
		return *this;
	}

	DescriptorPool::Builder& DescriptorPool::Builder::SetMaxSets(uint32_t count) {
		m_MaxSets = count;
		return *this;
	}

	std::unique_ptr<DescriptorPool> DescriptorPool::Builder::Build() const {
		return std::make_unique<DescriptorPool>(m_Device, m_MaxSets, m_PoolFlags, m_PoolSizes);
	}

	//DescriptorSetLayout
	DescriptorSetLayout::DescriptorSetLayout(Device& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
		: m_Device(device), m_Bindings(bindings)
	{
		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
		for (auto kv : m_Bindings) {
			setLayoutBindings.push_back(kv.second);
		}

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
		descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
		descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

		if (vkCreateDescriptorSetLayout(m_Device.Get(), &descriptorSetLayoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	DescriptorSetLayout::~DescriptorSetLayout() {
		vkDestroyDescriptorSetLayout(m_Device.Get(), m_DescriptorSetLayout, nullptr);
	}

	//DescriptorPool
	DescriptorPool::DescriptorPool(Device& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes) : m_Device(device) {
		VkDescriptorPoolCreateInfo descriptorPoolInfo{};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		descriptorPoolInfo.maxSets = maxSets;
		descriptorPoolInfo.flags = poolFlags;

		if (vkCreateDescriptorPool(m_Device.Get(), &descriptorPoolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	DescriptorPool::~DescriptorPool() {
		vkDestroyDescriptorPool(m_Device.Get(), m_DescriptorPool, nullptr);
	}

	bool DescriptorPool::AllocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const {
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPool;
		allocInfo.pSetLayouts = &descriptorSetLayout;
		allocInfo.descriptorSetCount = 1;

		if (vkAllocateDescriptorSets(m_Device.Get(), &allocInfo, &descriptor) != VK_SUCCESS) {
			return false;
		}
		return true;
	}

	void DescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const {
		vkFreeDescriptorSets(m_Device.Get(), m_DescriptorPool, static_cast<uint32_t>(descriptors.size()), descriptors.data());
	}

	void DescriptorPool::ResetPool() {
		vkResetDescriptorPool(m_Device.Get(), m_DescriptorPool, 0);
	}

	//DescriptorWriter
	DescriptorWriter::DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool) : m_SetLayout(setLayout), m_Pool(pool) {}

	DescriptorWriter& DescriptorWriter::WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo) {
		assert(m_SetLayout.m_Bindings.count(binding) == 1 && "Layout does not contain specified binding");
		auto& bindingDescription = m_SetLayout.m_Bindings[binding];
		assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pBufferInfo = bufferInfo;
		write.descriptorCount = 1;

		m_Writes.push_back(write);
		return *this;
	}

	DescriptorWriter& DescriptorWriter::WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo) {
		assert(m_SetLayout.m_Bindings.count(binding) == 1 && "Layout does not contain specified binding");
		auto& bindingDescription = m_SetLayout.m_Bindings[binding];
		assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pImageInfo = imageInfo;
		write.descriptorCount = 1;

		m_Writes.push_back(write);
		return *this;
	}

	bool DescriptorWriter::Build(VkDescriptorSet& set) {
		bool success = m_Pool.AllocateDescriptor(m_SetLayout.GetDescriptorSetLayout(), set);
		if (!success) {
			return false;
		}
		Overwrite(set);
		return true;
	}

	void DescriptorWriter::Overwrite(VkDescriptorSet& set) {
		for (auto& write : m_Writes) {
			write.dstSet = set;
		}
		vkUpdateDescriptorSets(m_Pool.m_Device.Get(), (uint32_t)m_Writes.size(), m_Writes.data(), 0, nullptr);
	}
}