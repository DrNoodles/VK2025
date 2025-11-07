#include "pch.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class HelloTriangleApplication;

static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	HelloTriangleApplication* context = static_cast<HelloTriangleApplication*>(pUserData);

	std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;

	return vk::False;
}

class HelloTriangleApplication
{
public:  // DATA
private: // DATA
	static constexpr int WIDTH{ 800 };
	static constexpr int HEIGHT{ 600 };
	
	static constexpr std::array<char const*, 1> m_validationLayers{
		"VK_LAYER_KHRONOS_validation"
	};

	#ifdef NDEBUG
	static constexpr bool m_enableValidationLayers = false;
	#else
	static constexpr bool m_enableValidationLayers = true;
	#endif

	GLFWwindow* m_window = nullptr;

	vk::raii::Context  m_context;
	vk::raii::Instance m_instance = nullptr;
	vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;

public:  // METHODS
	void Run()
	{
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private: // METHODS
	void initWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // no OGL - which is glfw's first love
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);   // we handle this ourselves

		m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", /*monitor*/nullptr, /*OGL crap*/nullptr);
	}

	std::vector<const char*> getRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (m_enableValidationLayers)
		{
			extensions.push_back(vk::EXTDebugUtilsExtensionName);
		}

		return extensions;
	}

	std::vector<const char*> getRequiredLayers()
	{
		std::vector<const char*> requiredLayers;

		if (m_enableValidationLayers)
		{
			requiredLayers.assign(m_validationLayers.begin(), m_validationLayers.end());
		}

		return requiredLayers;
	}

	void createInstance()
	{
		// Layers
		std::vector<char const*> requiredLayers = getRequiredLayers();
		{
			std::vector<vk::LayerProperties> availableLayers = m_context.enumerateInstanceLayerProperties();

			fmt::println("\nRequired Layers");
			for (auto& x : requiredLayers) { fmt::println("\t{}", x); }

			fmt::println("\nAvailable Layers");
			for (auto& x : availableLayers) { fmt::println("\t{}", x.layerName._Elems); }

			for (const char* required : requiredLayers)
			{
				if (std::ranges::none_of(availableLayers, [required](auto const& available) {
						return strcmp(available.layerName, required) == 0;
					}))
				{
					throw std::runtime_error("One or more required layers are not supported!");
				}
			}
		}

		// Extensions
		std::vector<char const*> requiredExtensions = getRequiredExtensions();
		{
			std::vector<vk::ExtensionProperties> availableExtensions = m_context.enumerateInstanceExtensionProperties();

			fmt::println("\nRequired Instance Extensions");
			for (auto& x : requiredExtensions) { fmt::println("\t{}", x); }

			fmt::println("\nAvailable Instance Extensions");
			for (auto& x : availableExtensions) { fmt::println("\t{}", x.extensionName._Elems); }

			for (const char* required : requiredExtensions)
			{
				if (std::ranges::none_of(availableExtensions, [required](auto const& available) {
						return strcmp(available.extensionName, required) == 0;
					}))
				{
					throw std::runtime_error("One or more required extensions are not supported!");
				}
			}
		}

		constexpr auto appInfo = vk::ApplicationInfo()
			.setPApplicationName("Hello Triangle")
			.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
			.setPEngineName("No Engine")
			.setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
			.setApiVersion(vk::ApiVersion14);

		const auto createInfo = vk::InstanceCreateInfo()
			.setPApplicationInfo(&appInfo)
			.setEnabledExtensionCount((u32)requiredExtensions.size())
			.setPpEnabledExtensionNames(requiredExtensions.data())
			.setEnabledLayerCount((u32)requiredLayers.size())
			.setPpEnabledLayerNames(requiredLayers.data());

		m_instance = vk::raii::Instance(m_context, createInfo);
	}

	void setupDebugMessenger()
	{
		if (!m_enableValidationLayers)
			return;

		using s = vk::DebugUtilsMessageSeverityFlagBitsEXT;
		using t = vk::DebugUtilsMessageTypeFlagBitsEXT;

		const auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT()
			.setMessageType(t::eGeneral | t::ePerformance | t::eValidation)// | t::eDeviceAddressBinding)
			.setMessageSeverity(s::eVerbose | s::eInfo | s::eWarning | s::eError)
			.setPfnUserCallback(&debugCallback)
			.setPUserData(this);

		m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(createInfo);
	}

	void initVulkan()
	{
		createInstance();
		setupDebugMessenger();
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