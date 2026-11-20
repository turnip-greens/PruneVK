#pragma once

#include "pvk_camera.hpp"

//lib
#include <vulkan/vulkan.h>

namespace pvk
{
	struct FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		PvkCamera& camera;
	};
}