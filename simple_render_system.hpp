#pragma once

#include "pvk_camera.hpp"
#include "pvk_pipeline.hpp"
#include "pvk_device.hpp"
#include "pvk_game_object.hpp"
#include "pvk_frame_info.hpp"

//std
#include <memory>
#include <vector>

namespace pvk
{
	class SimpleRenderSystem
	{
	public:

		SimpleRenderSystem(PvkDevice& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
		void renderGameObjects(FrameInfo& frameInfo, std::vector<PvkGameObject>& gameObjects);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		PvkDevice& pvkDevice;

		std::unique_ptr<PvkPipeline> pvkPipeline;
		VkPipelineLayout pipelineLayout;
		;
	};
}