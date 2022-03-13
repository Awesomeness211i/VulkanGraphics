#pragma once
#include <string>
#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Florencia {

	struct WindowProps {
		WindowProps(uint32_t width, uint32_t height, std::string title) :Title(title), Width(width), Height(height) {}
		std::string Title;
		bool Resized = false;
		uint32_t Width, Height;
	};

	class Window {
	public:
		Window(const WindowProps& props);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		void Update() { glfwPollEvents(); }

		VkExtent2D GetExtent() const { return { m_Properties.Width, m_Properties.Height }; }
		bool IsOpen() { return !glfwWindowShouldClose(m_Window); }
		bool WasResized() const { return m_Properties.Resized; }
		void ResetWindowResizeFlag() { m_Properties.Resized = false; }
		GLFWwindow* Get() const { return m_Window; }

		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		void InitializeWindow();
		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

		GLFWwindow* m_Window;
		WindowProps m_Properties;
	};

}
