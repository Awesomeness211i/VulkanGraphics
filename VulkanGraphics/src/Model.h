#pragma once
#include "Device.h"
#include "Buffer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <string_view>

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