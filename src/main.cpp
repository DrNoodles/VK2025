#include "pch.h"

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
	static constexpr int WIDTH{ 800 };
	static constexpr int HEIGHT{ 600 };
	
	GLFWwindow* m_window = nullptr;

	vk::raii::Context  m_context;
	vk::raii::Instance m_instance = nullptr;

private:
	void initWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // no OGL - which is glfw's first love
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);   // we handle this ourselves

		m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", /*monitor*/nullptr, /*OGL crap*/nullptr);
	}

	void createInstance()
	{
		constexpr auto appInfo = vk::ApplicationInfo()
			.setPApplicationName("Hello Triangle")
			.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
			.setPEngineName("No Engine")
			.setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
			.setApiVersion(vk::ApiVersion14);

		fmt::println("Instance Extension Properties");
		std::vector<vk::ExtensionProperties> extensionProperties = m_context.enumerateInstanceExtensionProperties();
		for (auto& ep : extensionProperties)
		{
			fmt::println("\t{}", ep.extensionName._Elems);
		}

		// Get GLFW extension for VK
		u32 glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		{
			// Check if the required GLFW extensions are supported by the Vulkan implementation.
			for (u32 i = 0; i < glfwExtensionCount; ++i)
			{
				if (std::ranges::none_of(extensionProperties,
					[glfwExtension = glfwExtensions[i]](const vk::ExtensionProperties& extensionProperty)
				{
					return strcmp(extensionProperty.extensionName, glfwExtension) == 0;
				}))
				{
					throw std::runtime_error("Required GLFW extension not supported: " + std::string(glfwExtensions[i]));
				}
			}
		}

		const auto createInfo = vk::InstanceCreateInfo()
			.setPApplicationInfo(&appInfo)
			.setEnabledExtensionCount(glfwExtensionCount)
			.setPpEnabledExtensionNames(glfwExtensions);

		m_instance = vk::raii::Instance(m_context, createInfo);
	}

	void initVulkan()
	{
		createInstance();
	}

	void mainLoop()
	{
		while (!glfwWindowShouldClose(m_window))
		{
			glfwPollEvents();

		}
	}

	void cleanup()
	{
		glfwDestroyWindow(m_window);
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