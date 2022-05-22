#include "Window.h"

namespace Florencia {

	Window::Window(const WindowProps& props) : m_Properties(props) {
		InitializeWindow();
	}

	Window::~Window() {
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, m_Window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to Create Window Surface");
		}
	}

	void Window::InitializeWindow() {
		if (glfwInit() != GL_FALSE) {
			glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			m_Window = glfwCreateWindow(m_Properties.Width, m_Properties.Height, m_Properties.Title.c_str(), nullptr, nullptr);
			glfwSetWindowUserPointer(m_Window, this);
			glfwSetFramebufferSizeCallback(m_Window, FramebufferResizeCallback);
		}
		if (!m_Window) {
			std::cout << "Window Creation Failed";
		}
	}

	void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto glfwWindowUserPointer = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		glfwWindowUserPointer->m_Properties.Resized = true;
		glfwWindowUserPointer->m_Properties.Width = width;
		glfwWindowUserPointer->m_Properties.Height = height;
	}

}