#pragma once

#include "Device.h"
#include "Window.h"
#include "Renderer.h"
#include "GameObject.h"

#include <memory>
#include <vector>

class Application {
public:
	Application();
	~Application();

	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	void Run();

private:
	void LoadGameObjects();

	Window m_Window{WindowProps()};
	Device m_Device{m_Window};
	Renderer m_Renderer{m_Window, m_Device};

	std::vector<GameObject> m_GameObjects;
};