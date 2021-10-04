module;
#include <chrono>
#include <vector>

#include <glm/glm.hpp>

#include <vulkan/vulkan.h>
export module Application;
import GameObject;
import Renderer;
import Window;
import Device;

namespace Florencia {

	struct GlobalUBO {
		glm::mat4 ProjectionView{ 1.0f };
		glm::vec3 LightDirection = glm::normalize(glm::vec3{ 1.0f, -3.0f, -1.0f });
	};

	export class Application {
	public:
		Application() { LoadGameObjects(); }
		~Application() {}

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void Run();

	private:
		void LoadGameObjects();

		Window m_Window{ WindowProps(800, 600, "Vulkan Tutorial") };
		Device m_Device{ m_Window };
		Renderer m_Renderer{ m_Window, m_Device };

		std::vector<GameObject> m_GameObjects;
	};

}

module: private;
import SimpleRenderSystem;
import ObjectController;
import FrameInfo;
import Buffer;
import Camera;
import Model;

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

		SimpleRenderSystem simpleRenderSystem(m_Device, m_Renderer.GetSwapChainRenderPass());
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
					frameIndex,
					timeStep,
					commandBuffer,
					camera
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

		vkDeviceWaitIdle(m_Device.device());
	}

}