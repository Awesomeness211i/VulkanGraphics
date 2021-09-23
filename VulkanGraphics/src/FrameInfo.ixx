module;
#include <vulkan/vulkan.h>
export module FrameInfo;
import Camera;

export namespace Florencia {

	struct FrameInfo {
		int FrameIndex;
		float FrameTime;
		VkCommandBuffer CommandBuffer;
		Camera& Camera;
	};

}