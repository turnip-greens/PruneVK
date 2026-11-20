#pragma once

#include "pvk_window.hpp"
#include "pvk_device.hpp"
#include "pvk_swap_chain.hpp"

//std
#include <memory>
#include <vector>
#include <cassert>

namespace pvk
{
	class PvkRenderer
	{
	public:

		PvkRenderer(PvkWindow& window, PvkDevice& device);
		~PvkRenderer();

		PvkRenderer(const PvkRenderer&) = delete;
		PvkRenderer& operator=(const PvkRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return pvkSwapChain->getRenderPass(); }
		float getAspectRatio() const { return pvkSwapChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const 
		{ 
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const
		{
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();

		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);


	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		PvkWindow& pvkWindow;
		PvkDevice& pvkDevice;
		std::unique_ptr<PvkSwapChain> pvkSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;
		
		uint32_t currentImageIndex;
		int currentFrameIndex;
		bool isFrameStarted;
	};
}