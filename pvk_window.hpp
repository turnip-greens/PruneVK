#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace pvk
{
	class PvkWindow
	{
	public:
		PvkWindow(int w, int h, std::string name);
		~PvkWindow();

		PvkWindow(const PvkWindow&) = delete;
		PvkWindow& operator=(const PvkWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }
		void close() { glfwSetWindowShouldClose(window, GLFW_TRUE); }
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
		bool wasWindowResized() { return framebufferResized; }
		void resetWindowResizedFlag() { framebufferResized = false; }
		GLFWwindow* getGLFWwindow() const { return window; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();

		int width;
		int height;
		bool framebufferResized = false;
		
		std::string windowName;

		GLFWwindow* window;
	};
}