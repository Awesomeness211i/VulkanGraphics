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
	camera.SetViewTarget(glm::vec3(-1.0f, -2.0f, -2.0f), glm::vec3(0.0f, 0.0f, 2.5f));

	auto viewer = GameObject::CreateGameObject();
	ObjectController cameraController{};

	auto currentTime = std::chrono::high_resolution_clock::now();

	while (m_Window.IsOpen()) {
		glfwPollEvents();

		auto newTime = std::chrono::high_resolution_clock::now();
		float timestep = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;
		timestep = glm::min(timestep, 1.0f);

		cameraController.MoveInPlaneXZ(m_Window.Get(), timestep, viewer);
		camera.SetViewYXZ(viewer.m_Transform.translation, viewer.m_Transform.rotation);

		float aspect = m_Renderer.GetAspectRatio();
		//camera.SetOrthographicProjection(-aspect, aspect, -1.0f, 1.0f, -5.0f, 5.0f);
		camera.SetPerspectiveProjection(glm::radians(70.0f), aspect, 0.1f, 10.0f);
		
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
	std::shared_ptr<Model> model = Model::CreateModelFromFile(m_Device, "assets/models/smooth_vase.obj");
	auto smoothVase = GameObject::CreateGameObject();
	smoothVase.m_Model = model;
	smoothVase.m_Transform.translation = { 0.5f, -1.0f, 1.0f };
	smoothVase.m_Transform.scale *= 1.0f;
	model.reset();

	model = Model::CreateModelFromFile(m_Device, "assets/models/flat_vase.obj");
	auto flatVase = GameObject::CreateGameObject();
	flatVase.m_Model = model;
	flatVase.m_Transform.translation = { -0.5f, -1.0f, 1.0f };
	flatVase.m_Transform.scale *= 1.0f;
	model.reset();

	model = Model::CreateModelFromFile(m_Device, "assets/models/colored_cube.obj");
	auto coloredCube = GameObject::CreateGameObject();
	coloredCube.m_Model = model;
	coloredCube.m_Transform.translation = { 0.0f, 0.0f, 0.0f };
	coloredCube.m_Transform.scale *= 1.0f;
	model.reset();

	model = Model::CreateModelFromFile(m_Device, "assets/models/cube.obj");
	auto cube = GameObject::CreateGameObject();
	cube.m_Model = model;
	cube.m_Transform.translation = { -1.0f, 0.0f, 0.0f };
	cube.m_Transform.scale *= 1.0f;
	model.reset();

	m_GameObjects.push_back(std::move(smoothVase));
	m_GameObjects.push_back(std::move(flatVase));

	m_GameObjects.push_back(std::move(coloredCube));
	m_GameObjects.push_back(std::move(cube));
}