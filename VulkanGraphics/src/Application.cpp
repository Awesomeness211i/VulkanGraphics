#include "Application.h"

#include "Camera.h"
#include "ObjectController.h"
#include "SimpleRenderSystem.h"

#include <chrono>
#include <iostream>

Application::Application() {
	LoadGameObjects();
}

Application::~Application() {}

void Application::Run() {
	SimpleRenderSystem simpleRenderSystem(m_Device, m_Renderer.GetSwapChainRenderPass());
	Camera camera{};

	auto viewer = GameObject::CreateGameObject();
	ObjectController cameraController{};

	ObjectController cubeController{};

	auto currentTime = std::chrono::high_resolution_clock::now();
	while (m_Window.IsOpen()) {
		glfwPollEvents();

		auto newTime = std::chrono::high_resolution_clock::now();
		float timestep = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;
		timestep = glm::min(timestep, 1.0f);

		cameraController.MoveInPlaneXZ(m_Window.Get(), timestep, viewer);
		camera.SetViewYXZ(viewer.m_Transform.translation, viewer.m_Transform.rotation);
		cubeController.MoveInPlaneXZ(m_Window.Get(), timestep, m_GameObjects[0]);

		float aspect = m_Renderer.GetAspectRatio();
		camera.SetPerspectiveProjection(glm::radians(70.0f), aspect, 0.1f, 100.0f);

		if (auto commandBuffer = m_Renderer.BeginFrame()) {
			m_Renderer.BeginSwapChainRenderPass(commandBuffer);
			simpleRenderSystem.RenderGameObjects(commandBuffer, m_GameObjects, camera);
			m_Renderer.EndSwapChainRenderPass(commandBuffer);
			m_Renderer.EndFrame();
		}
	}

	vkDeviceWaitIdle(m_Device.device());
}

void Application::LoadGameObjects() {
	std::shared_ptr<Model> model = Model::CreateModelFromFile(m_Device, "assets/models/colored_cube.obj");
	auto cube = GameObject::CreateGameObject();
	cube.m_Model = model;
	cube.m_Transform.translation = { 0.0, 1.0, 5.0 };
	cube.m_Transform.scale *= 1.0f;

	auto cube2 = GameObject::CreateGameObject();
	cube2.m_Model = model;
	cube2.m_Transform.translation = { 1.0, 1.0, 5.0 };
	cube2.m_Transform.scale *= 1.0f;

	m_GameObjects.push_back(std::move(cube));
	m_GameObjects.push_back(std::move(cube2));
}