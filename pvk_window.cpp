#include "pvk_window.hpp"

#include <stdexcept>

namespace pvk
{
	PvkWindow::PvkWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } 
	{
		initWindow();
	}

	PvkWindow::~PvkWindow()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	
	void PvkWindow::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void PvkWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create window surface");
		}
	}

	void PvkWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto pvkWindow = reinterpret_cast<PvkWindow*> (glfwGetWindowUserPointer(window));
		pvkWindow->framebufferResized = true;
		pvkWindow->width = width;
		pvkWindow->height = height;
	}
}