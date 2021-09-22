#pragma once
#include "Camera.h"

#include <vulkan/vulkan.h>

struct FrameInfo {
	int FrameIndex;
	float FrameTime;
	VkCommandBuffer CommandBuffer;
	Camera& Camera;
};