#include "Application.h"
#include <glm/glm.hpp>
#include <chrono>

#include "Systems/SimpleRenderSystem.h"
#include "Systems/PointLightSystem.h"
#include "ObjectController.h"
#include "FrameInfo.h"
#include "Camera.h"
#include "Buffer.h"
#include "Model.h"

namespace Florencia {

	Application::Application() {
		m_GlobalPool = DescriptorPool::Builder(m_Device)
			.SetMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.Build();
		LoadGameObjects();
	}

	void Application::LoadGameObjects() {
		auto model = Model::CreateModelFromFile(m_Device, "assets/models/cube.obj");
		auto cube = GameObject::CreateGameObject();
		cube.m_Transform.translation = { -1.0f, 0.0f, 0.0f };
		cube.m_Transform.scale *= 1.0f;
		cube.m_Model = model;
		m_GameObjects.emplace(cube.GetID(), std::move(cube));

		model = Model::CreateModelFromFile(m_Device, "assets/models/colored_cube.obj");
		auto colorcube = GameObject::CreateGameObject();
		colorcube.m_Transform.translation = { 1.0f, 0.0f, 0.0f };
		colorcube.m_Transform.scale *= 1.0f;
		colorcube.m_Model = model;
		m_GameObjects.emplace(colorcube.GetID(), std::move(colorcube));

		model = Model::CreateModelFromFile(m_Device, "assets/models/quad.obj");
		auto floor = GameObject::CreateGameObject();
		floor.m_Transform.translation = { 0.0f, 0.5f, 0.0f };
		floor.m_Transform.scale *= 2.0f;
		floor.m_Model = model;
		m_GameObjects.emplace(floor.GetID(), std::move(floor));

		model = Model::CreateModelFromFile(m_Device, "assets/models/flat_vase.obj");
		auto flat_vase = GameObject::CreateGameObject();
		flat_vase.m_Transform.translation = { -1.0f, -0.5f, 0.0f };
		flat_vase.m_Transform.scale *= 2.0f;
		flat_vase.m_Model = model;
		m_GameObjects.emplace(flat_vase.GetID(), std::move(flat_vase));

		model = Model::CreateModelFromFile(m_Device, "assets/models/smooth_vase.obj");
		auto smooth_vase = GameObject::CreateGameObject();
		smooth_vase.m_Transform.translation = { 1.0f, -0.5f, 0.0f };
		smooth_vase.m_Transform.scale *= 2.0f;
		smooth_vase.m_Model = model;
		m_GameObjects.emplace(smooth_vase.GetID(), std::move(smooth_vase));

		auto pointLight1 = GameObject::CreatePointLight(1.0f, 0.2f, {0.4f, 0.0f, 0.9f});
		pointLight1.m_Transform.translation = { 0.0f, -1.0f, 1.0f };
		m_GameObjects.emplace(pointLight1.GetID(), std::move(pointLight1));

		auto pointLight2 = GameObject::CreatePointLight(0.5);
		pointLight2.m_Transform.translation = { 0.0f, -1.0f, -1.0f };
		m_GameObjects.emplace(pointLight2.GetID(), std::move(pointLight2));
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
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.Build();

		std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->DescriptorInfo();
			DescriptorWriter(*globalSetLayout, *m_GlobalPool)
				.WriteBuffer(0, &bufferInfo)
				.Build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem(m_Device, m_Renderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout());
		PointLightSystem pointLightSystem(m_Device, m_Renderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout());
		Camera camera{};

		auto viewer = GameObject::CreateGameObject();
		ObjectController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (m_Window.IsOpen()) {
			m_Window.Update();

			auto newTime = std::chrono::high_resolution_clock::now();
			float timeStep = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.MoveInPlaneXZ(m_Window.Get(), timeStep, viewer);
			camera.SetViewYXZ(viewer.m_Transform.translation, viewer.m_Transform.rotation);

			float aspect = m_Renderer.GetAspectRatio();
			camera.SetPerspectiveProjection(glm::radians(70.0f), aspect, 0.01f, 100.0f);

			if (auto commandBuffer = m_Renderer.BeginFrame()) {
				int frameIndex = m_Renderer.GetFrameIndex();
				FrameInfo frameInfo {
					camera,
					frameIndex,
					timeStep,
					commandBuffer,
					globalDescriptorSets[frameIndex],
					m_GameObjects
				};

				//Update
				GlobalUBO ubo{};
				ubo.m_ProjectionMatrix = camera.GetProjectionMatrix();
				ubo.m_ViewMatrix = camera.GetViewMatrix();
				ubo.m_InverseViewMatrix = camera.GetInverseViewMatrix();
				pointLightSystem.Update(frameInfo, ubo);
				uboBuffers[frameIndex]->WriteToBuffer(&ubo);
				uboBuffers[frameIndex]->Flush();

				//Render
				m_Renderer.BeginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.RenderGameObjects(frameInfo);
				pointLightSystem.Render(frameInfo);
				m_Renderer.EndSwapChainRenderPass(commandBuffer);
				m_Renderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(m_Device.Get());
	}
}
