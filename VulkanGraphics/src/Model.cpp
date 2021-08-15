#include "Model.h"
#include <cassert>
#include <unordered_map>

#include "Utilities.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../vendor/TinyObjLoader.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace std {

	template <>
	struct hash<Model::Vertex> {
		size_t operator()(const Model::Vertex& vertex) const {
			size_t seed = 0;
			HashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};

}

void Model::Data::LoadModel(const std::string_view filepath) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, error;

	if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, filepath.data())) {
		throw std::runtime_error(warn + error);
	}

	vertices.clear();
	indices.clear();
	std::unordered_map<Vertex, uint32_t> uniqueVertices{};
	for(const auto& shape : shapes) {
		for(const auto& index : shape.mesh.indices) {
			Vertex vertex{};
			if(index.vertex_index >= 0) {
				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2],
					1.0f
				};

				vertex.color = {
					attrib.colors[3 * index.vertex_index + 0],
					attrib.colors[3 * index.vertex_index + 1],
					attrib.colors[3 * index.vertex_index + 2],
					1.0f
				};
			}

			if(index.normal_index >= 0) {
				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2],
					0.0f
				};
			}

			if(index.texcoord_index >= 0) {
				vertex.uv = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1]
				};
			}

			if(uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(std::move(vertex));
			}
			indices.push_back(uniqueVertices[vertex]);
		}
	}
}

Model::Model(Device& device, const Data& builder) : m_Device{device} { CreateBuffers(builder); }

Model::~Model() {
	vkDestroyBuffer(m_Device.device(), m_VertexBuffer, nullptr);
	vkFreeMemory(m_Device.device(), m_VertexBufferMemory, nullptr);

	if(m_HasIndexBuffer) {
		vkDestroyBuffer(m_Device.device(), m_IndexBuffer, nullptr);
		vkFreeMemory(m_Device.device(), m_IndexBufferMemory, nullptr);
	}
}

std::unique_ptr<Model> Model::CreateModelFromFile(Device& device, const std::string_view filepath) {
	Data data{};
	data.LoadModel(filepath);
	return std::make_unique<Model>(device, data);
}

void Model::Bind(VkCommandBuffer commandBuffer) {
	VkBuffer buffers[] = { m_VertexBuffer };
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

	if(m_HasIndexBuffer) { vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32); }
}

void Model::Draw(VkCommandBuffer commandBuffer) {
	if(m_HasIndexBuffer) { vkCmdDrawIndexed(commandBuffer, m_IndexCount, 1, 0, 0, 0); }
	else { vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0); }
}

template <typename T>
void AllocateBuffer(Device& device, const std::vector<T>& inBuffer, VkBuffer& outBuffer, VkDeviceMemory& outMemory, VkBufferUsageFlagBits bit) {
	VkDeviceSize bufferSize = sizeof(inBuffer[0]) * static_cast<uint32_t>(inBuffer.size());

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	device.createBuffer(bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory
	);

	void* data;
	vkMapMemory(device.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, inBuffer.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(device.device(), stagingBufferMemory);

	device.createBuffer(bufferSize,
		bit | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		outBuffer,
		outMemory
	);

	device.copyBuffer(stagingBuffer, outBuffer, bufferSize);

	vkDestroyBuffer(device.device(), stagingBuffer, nullptr);
	vkFreeMemory(device.device(), stagingBufferMemory, nullptr);
}

void Model::CreateBuffers(const Data& builder) {
	m_VertexCount = static_cast<uint32_t>(builder.vertices.size());
	assert(m_VertexCount >= 3 && "Vertex Count Must Be At Least 3");
	if(m_VertexCount >= 3) { AllocateBuffer(m_Device, builder.vertices, m_VertexBuffer, m_VertexBufferMemory, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT); }

	m_IndexCount = static_cast<uint32_t>(builder.indices.size());
	if(m_HasIndexBuffer = m_IndexCount > 0) { AllocateBuffer(m_Device, builder.indices, m_IndexBuffer, m_IndexBufferMemory, VK_BUFFER_USAGE_INDEX_BUFFER_BIT); }
}

std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescriptions() {
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttributeDescriptions() {
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

	//location, binding, format, offset
	attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, position) });
	attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, color) });
	attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, normal) });
	attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

	return attributeDescriptions;
}