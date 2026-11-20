#include "pvk_renderer.hpp"

//std
#include <stdexcept>
#include <array>

namespace pvk
{
	PvkRenderer::PvkRenderer(PvkWindow& window, PvkDevice& device) : pvkWindow{ window }, pvkDevice{ device }
	{
		currentImageIndex = 0;
		currentFrameIndex = 0;
		isFrameStarted = false;

		recreateSwapChain();
		createCommandBuffers();
	}

	PvkRenderer::~PvkRenderer()
	{
		pvkSwapChain.reset();
		freeCommandBuffers();
	}

	void PvkRenderer::recreateSwapChain()
	{
		auto extent = pvkWindow.getExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = pvkWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(pvkDevice.device());

		if (pvkSwapChain == nullptr)
		{
			pvkSwapChain = std::make_unique<PvkSwapChain>(pvkDevice, extent);
		}
		else
		{
			std::shared_ptr<PvkSwapChain> oldSwapChain = std::move(pvkSwapChain);
			pvkSwapChain = std::make_unique<PvkSwapChain>(pvkDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*pvkSwapChain.get()))
			{
				throw std::runtime_error("Swap chain image(or depth) format has changed!");
			}
		}
	}

	void PvkRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			pvkDevice.device(),
			pvkDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data()
		);
		commandBuffers.clear();
	}

	void PvkRenderer::createCommandBuffers()
	{
		commandBuffers.resize(PvkSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = pvkDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(pvkDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	VkCommandBuffer PvkRenderer::beginFrame()
	{
		assert(!isFrameStarted && "cannot call begin frame when frame is already started!");

		auto result = pvkSwapChain->acquireNextImage(&currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to aquire swap chain image!");
		}

		isFrameStarted = true;
		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		return commandBuffer;
	}
	void PvkRenderer::endFrame()
	{
		assert(isFrameStarted && "Can't call end frane when a frame has not started!");
		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = pvkSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || pvkWindow.wasWindowResized())
		{
			pvkWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}

		// Fix: Handle these cases separately
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || pvkWindow.wasWindowResized())
		{
			pvkWindow.resetWindowResizedFlag();
			recreateSwapChain();
			isFrameStarted = false;
			return; // Exit early - frame was recreated
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % PvkSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void PvkRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't begin swap chain render pass when a frame has not started!");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame!");


		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = pvkSwapChain->getRenderPass();
		renderPassInfo.framebuffer = pvkSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = pvkSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(pvkSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(pvkSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, pvkSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}
	void PvkRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't end swap chain render pass when a frame has not started!");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame!");

		vkCmdEndRenderPass(commandBuffer);
	}
}