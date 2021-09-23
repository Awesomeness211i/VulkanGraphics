module;
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define TINYOBJLOADER_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL
#include <memory>
#include <vector>
#include <cassert>
#include <functional>
#include <string_view>
#include <unordered_map>


#include "../vendor/TinyObjLoader.h"
#include <glm/gtx/hash.hpp>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
export module Model;
import Utilities;
import Device;
import Buffer;

export namespace Florencia {

	class Model {
	public:
		struct Vertex {
			glm::vec4 position;
			glm::vec4 color;
			glm::vec4 normal;
			glm::vec2 uv;

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

			bool operator==(const Vertex& other) const {
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Data {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void LoadModel(const std::string_view filepath);
		};

		Model(Device& device, const Data& builder);
		~Model();

		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;

		static std::unique_ptr<Model> CreateModelFromFile(Device& device, const std::string_view filepath);

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

	private:
		void AllocateVertexBuffers(const std::vector<Vertex>& vertices);
		void AllocateIndexBuffers(const std::vector<uint32_t>& indices);

		Device& m_Device;
		bool m_HasIndexBuffer{ false };
		uint32_t m_VertexCount, m_IndexCount;
		std::unique_ptr<Buffer> m_VertexBuffer, m_IndexBuffer;
	};

}

module: private;

namespace std {

	template <>
	struct hash<Florencia::Model::Vertex> {
		size_t operator()(const Florencia::Model::Vertex& vertex) const {
			size_t seed = 0;
			Florencia::HashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};

}

namespace Florencia {

	void Model::Data::LoadModel(const std::string_view filepath) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, error;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, filepath.data())) {
			throw std::runtime_error(warn + error);
		}

		vertices.clear();
		indices.clear();
		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};
				if (index.vertex_index >= 0) {
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

				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
						0.0f
					};
				}

				if (index.texcoord_index >= 0) {
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(std::move(vertex));
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

	Model::Model(Device& device, const Data& builder) : m_Device{ device } {
		AllocateVertexBuffers(builder.vertices);
		AllocateIndexBuffers(builder.indices);
	}

	Model::~Model() {}

	std::unique_ptr<Model> Model::CreateModelFromFile(Device& device, const std::string_view filepath) {
		Data data{};
		data.LoadModel(filepath);
		return std::make_unique<Model>(device, data);
	}

	void Model::Bind(VkCommandBuffer commandBuffer) {
		VkBuffer buffers[] = { m_VertexBuffer->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (m_HasIndexBuffer) { vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32); }
	}

	void Model::Draw(VkCommandBuffer commandBuffer) {
		if (m_HasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, m_IndexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
		}
	}

	void Model::AllocateVertexBuffers(const std::vector<Vertex>& vertices) {
		m_VertexCount = static_cast<uint32_t>(vertices.size());
		assert(m_VertexCount >= 3 && "Vertex Count Must Be At Least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;
		uint32_t elementSize = sizeof(vertices[0]);

		Buffer stagingBuffer{
			m_Device,
			elementSize,
			m_VertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};
		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer((void*)vertices.data());

		m_VertexBuffer = std::make_unique<Buffer>(
			m_Device,
			elementSize,
			m_VertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

		m_Device.copyBuffer(stagingBuffer.GetBuffer(), m_VertexBuffer->GetBuffer(), bufferSize);
	}

	void Model::AllocateIndexBuffers(const std::vector<uint32_t>& indices) {
		m_IndexCount = static_cast<uint32_t>(indices.size());
		m_HasIndexBuffer = m_IndexCount > 0;

		if (!m_HasIndexBuffer) { return; }

		VkDeviceSize bufferSize = sizeof(indices[0]) * m_IndexCount;
		uint32_t elementSize = sizeof(indices[0]);

		Buffer stagingBuffer{
			m_Device,
			elementSize,
			m_IndexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer((void*)indices.data());

		m_IndexBuffer = std::make_unique<Buffer>(
			m_Device,
			elementSize,
			m_IndexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_Device.copyBuffer(stagingBuffer.GetBuffer(), m_IndexBuffer->GetBuffer(), bufferSize);
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

}