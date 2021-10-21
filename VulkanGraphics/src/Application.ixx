module;
#include <chrono>
#include <vector>

#include <glm/glm.hpp>

#include <vulkan/vulkan.h>
export module Application;
import SimpleRenderSystem;
import ObjectController;
import Descriptors;
import GameObject;
import FrameInfo;
import Renderer;
import Camera;
import Buffer;
import Window;
import Device;
import Model;

namespace Florencia {

	struct GlobalUBO {
		glm::mat4 ProjectionView{ 1.0f };
		glm::vec3 LightDirection = glm::normalize(glm::vec3{ 1.0f, -3.0f, -1.0f });
	};

	export class Application {
	public:
		Application() {
			m_GlobalPool = DescriptorPool::Builder(m_Device)
				.SetMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
				.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
				.Build();
			LoadGameObjects();
		}
		~Application() {}

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void Run();

	private:
		void LoadGameObjects();

		Window m_Window{ WindowProps(800, 600, "Vulkan Tutorial") };
		Device m_Device{ m_Window };
		Renderer m_Renderer{ m_Window, m_Device };

		std::unique_ptr<DescriptorPool> m_GlobalPool{};
		std::vector<GameObject> m_GameObjects;
	};

}

module: private;

namespace Florencia {

	void Application::LoadGameObjects() {
		auto model = Model::CreateModelFromFile(m_Device, "assets/models/colored_cube.obj");
		auto cube = GameObject::CreateGameObject();
		cube.m_Transform.translation = { 0.0, 0.0, 2.0 };
		cube.m_Transform.scale *= 1.0f;
		cube.m_Model = model;
		m_GameObjects.push_back(std::move(cube));
	}

	void Application::Run() {
		std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<Buffer>(
				m_Device,
				sizeof(GlobalUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			uboBuffers[i]->Map();
		}

		auto globalSetLayout = DescriptorSetLayout::Builder(m_Device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
			.Build();

		std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->DescriptorInfo();
			DescriptorWriter(*globalSetLayout, *m_GlobalPool)
				.WriteBuffer(0, &bufferInfo)
				.Build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem(m_Device, m_Renderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout());
		Camera camera{};

		auto viewer = GameObject::CreateGameObject();
		ObjectController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();
		while (m_Window.IsOpen()) {
			m_Window.Update();

			auto newTime = std::chrono::high_resolution_clock::now();
			float timeStep = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;
			timeStep = glm::min(timeStep, 1.0f);

			cameraController.MoveInPlaneXZ(m_Window.Get(), timeStep, viewer);
			camera.SetViewYXZ(viewer.m_Transform.translation, viewer.m_Transform.rotation);

			float aspect = m_Renderer.GetAspectRatio();
			camera.SetPerspectiveProjection(glm::radians(70.0f), aspect, 0.1f, 100.0f);

			if (auto commandBuffer = m_Renderer.BeginFrame()) {
				int frameIndex = m_Renderer.GetFrameIndex();
				FrameInfo frameInfo{
					camera,
					frameIndex,
					timeStep,
					commandBuffer,
					globalDescriptorSets[frameIndex]
				};

				//Update
				GlobalUBO ubo{};
				ubo.ProjectionView = camera.GetProjectionMatrix() * camera.GetViewMatrix();
				uboBuffers[frameIndex]->WriteToBuffer(&ubo);
				uboBuffers[frameIndex]->Flush();

				//Render
				m_Renderer.BeginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.RenderGameObjects(frameInfo, m_GameObjects);
				m_Renderer.EndSwapChainRenderPass(commandBuffer);
				m_Renderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(m_Device.Get());
	}

}