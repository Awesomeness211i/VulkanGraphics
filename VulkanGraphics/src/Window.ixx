module;
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
export module Window;
import <string>;

export namespace Florencia {

	struct WindowProps {
		WindowProps(uint32_t width, uint32_t height, std::string title) :Width(width), Height(height), Title(title) {}
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

module: private;
import <iostream>;

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
		auto t_Window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		t_Window->m_Properties.Resized = true;
		t_Window->m_Properties.Width = width;
		t_Window->m_Properties.Height = height;
	}

}