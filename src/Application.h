#pragma once
#include <memory>

#include "Descriptors.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Window.h"
#include "Device.h"

namespace Florencia {

	class Application {
	public:
		Application();
		~Application() {}

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void Run();

	private:
		void LoadGameObjects();

		Window m_Window{WindowProps(800, 600, "Vulkan Tutorial")};
		Device m_Device{m_Window};
		Renderer m_Renderer{m_Window, m_Device};

		std::unique_ptr<DescriptorPool> m_GlobalPool{};
		GameObject::Map_t m_GameObjects;
	};

}
