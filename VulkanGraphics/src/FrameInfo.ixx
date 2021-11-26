module;
#include <vulkan/vulkan.h>
export module FrameInfo;
export import GameObject;
export import Camera;

export namespace Florencia {

	struct FrameInfo {
		Camera& Camera;
		int FrameIndex;
		float FrameTime;
		VkCommandBuffer CommandBuffer;
		VkDescriptorSet GlobalDescriptorSet;
		GameObject::Map_t& GameObjects;
	};

}