#include "Descriptors.h"
#include <stdexcept>

namespace Florencia {

	//DescriptorSetLayout Builder
	DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count) {
		if(m_Bindings.count(binding) != 0) {
			throw std::runtime_error("Binding already in use");
		}
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
	DescriptorWriter::DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool) : m_Pool(pool), m_SetLayout(setLayout) {}

	DescriptorWriter& DescriptorWriter::WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo) {
		if(m_SetLayout.m_Bindings.count(binding) != 1) {
			throw std::runtime_error("Layout does not contain specified binding");
		}
		auto& bindingDescription = m_SetLayout.m_Bindings[binding];
		if(bindingDescription.descriptorCount != 1) {
			throw std::runtime_error("Binding single descriptor info, but binding expects multiple");
		}

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
		if(m_SetLayout.m_Bindings.count(binding) != 1) {
			throw std::runtime_error("Layout does not contain specified binding");
		}
		auto& bindingDescription = m_SetLayout.m_Bindings[binding];
		if(bindingDescription.descriptorCount != 1) {
			throw std::runtime_error("Binding single descriptor info, but binding expects multiple");
		}

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
