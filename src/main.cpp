#include <vulkan/vulkan_raii.hpp>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <fmt/base.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


class HelloTriangleApplication
{
public:
	void Run()
	{
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	GLFWwindow* window = nullptr;
	static constexpr int WIDTH { 800 };
	static constexpr int HEIGHT { 600 };

private:
	void initWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // no OGL - which is glfw's first love
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);   // we handle this ourselves

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", /*monitor*/nullptr, /*OGL crap*/nullptr);
	}

	void initVulkan()
	{


	}

	void mainLoop()
	{
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}
	}

	void cleanup()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}
};

int main()
{
	HelloTriangleApplication app;

	try
	{
		app.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}