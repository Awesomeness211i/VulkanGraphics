module;
#include <vulkan/vulkan.h>
export module FrameInfo;
import Camera;

export namespace Florencia {

	struct FrameInfo {
		Camera& Camera;
		int FrameIndex;
		float FrameTime;
		VkCommandBuffer CommandBuffer;
		VkDescriptorSet GlobalDescriptorSet;
	};

}