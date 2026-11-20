#pragma once

#include "pvk_device.hpp"
#include "pvk_buffer.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

//std
#include <vector>
#include <memory>

namespace pvk
{
	class PvkModel
	{
	public:
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;
			glm::vec3 normal;
			glm::vec2 uv;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& o) const
			{
				return position == o.position && color == o.color && normal == o.normal && uv == o.uv;
			}
		};

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);
		};

		PvkModel(PvkDevice &device, const PvkModel::Builder& builder);
		~PvkModel();

		PvkModel(const PvkModel&) = delete;
		PvkModel& operator=(const PvkModel&) = delete;

		static std::unique_ptr<PvkModel> createModelFromFile(PvkDevice& device, const std::string& filepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		PvkDevice& pvkDevice;

		std::unique_ptr<PvkBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		std::unique_ptr<PvkBuffer> indexBuffer;
		uint32_t indexCount;
	};
}