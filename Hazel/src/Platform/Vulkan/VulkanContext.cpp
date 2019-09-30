#include "hzpch.h"
#include "VulkanContext.h"
//#include "Platform/Windows/WindowsWindow.h"
#include "Platform/Vulkan/VulkanUtility.h"

//#include <glad/glad.h>


namespace Hazel {

	VulkanContext* VulkanContext::s_Context = nullptr;

	VulkanContext::VulkanContext(GLFWwindow* windowHandle)
		:m_WindowHandle(windowHandle)
	{
		HZ_CORE_ASSERT(!s_Context, "VulkanContext already exsists!");
		s_Context = this;
		HZ_CORE_ASSERT(windowHandle, "Window handle is null");
	}

	VulkanContext::~VulkanContext()
	{
		m_SwapChain->Destroy();
		vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, nullptr);
		vkDestroyDevice(m_LogicalDevice, nullptr);
		if (m_EnableValidationLayers)
		{
			auto DestroyDebugUtilsMessengerEXT =
				(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
			DestroyDebugUtilsMessengerEXT(m_Instance, m_debugMessenger, nullptr);
		}
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}

	void VulkanContext::Init()
	{
		VkResult result;
		uint32_t vulkanVersion;
		result = vkEnumerateInstanceVersion(&vulkanVersion);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Vulkan version >= 1.1.0 not available: {0}", result);
		
		CreateInstance(&vulkanVersion);
		SetupDebugMessenger();
		CreateSurface();
		SelectPhysicalDevice();
		CreateLogicalDevice();
		CreateCommandPool();
		m_SwapChain.reset(new VulkanSwapChain(m_WindowHandle, m_PhysicalDevice, m_LogicalDevice, m_Surface));

		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &physicalDeviceProperties);

		HZ_CORE_INFO("Vulkan Info:");
//		HZ_CORE_INFO("  Vendor: {0}", physicalDeviceProperties.vendorID);
		HZ_CORE_INFO("  Renderer: {0}", physicalDeviceProperties.deviceName);
		HZ_CORE_INFO("  Version: {0}.{1}.{2}", VK_VERSION_MAJOR(vulkanVersion),
			VK_VERSION_MINOR(vulkanVersion), VK_VERSION_PATCH(vulkanVersion));

//		HZ_CORE_ASSERT(false, "Vulkan implementation is not complete!");
	}

	void VulkanContext::SwapBuffers()
	{
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanContext::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageSeverityFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT * callbackData,
		void * userData)
	{
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			HZ_CORE_TRACE("Vulkan Validation: {0}", callbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			HZ_CORE_INFO("Vulkan Validation: {0}", callbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			HZ_CORE_WARN("Vulkan Validation: {0}", callbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			HZ_CORE_ERROR("Vulkan Validation: {0}", callbackData->pMessage);
			break;
		}

		return VK_FALSE;
	}

	void VulkanContext::CreateInstance(uint32_t* vulkanVersion)
	{
		VkResult result;

		VkApplicationInfo applicationInfo = {};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pNext = NULL;
		applicationInfo.pApplicationName = "Sandbox";
		applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
		applicationInfo.pEngineName = "Hazel Engine";
		applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
		applicationInfo.apiVersion = *vulkanVersion;

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext = NULL;
		instanceCreateInfo.flags = 0;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;

		if (m_EnableValidationLayers)
		{
			HZ_CORE_ASSERT(CheckValidationLayerSupport(), "Requested Vulkan validation layers not available!");
			instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
			instanceCreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();
		}
		else
		{
			instanceCreateInfo.enabledLayerCount = 0;
			instanceCreateInfo.ppEnabledLayerNames = NULL;
		}

		auto extensions = GetRequiredExtensions();
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

		result = vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Unable to create Vulkan instance: {0}", result);

	}

	bool VulkanContext::CheckValidationLayerSupport()
	{
		
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : m_ValidationLayers) 
		{
			bool layerFound = false;
			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}
			if (!layerFound)
				return false;
		}
		
		return true;
	}

	std::vector<const char*> VulkanContext::GetRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (m_EnableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	void VulkanContext::SetupDebugMessenger()
	{
		if (!m_EnableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
		debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugMessengerCreateInfo.pNext = NULL;
		debugMessengerCreateInfo.flags = 0;
		debugMessengerCreateInfo.messageSeverity = 
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugMessengerCreateInfo.pfnUserCallback = DebugCallback;
		debugMessengerCreateInfo.pUserData = nullptr;

		auto CreateDebugUtilsMessengerEXT =
			(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
		VkResult result = CreateDebugUtilsMessengerEXT(m_Instance, &debugMessengerCreateInfo, nullptr, &m_debugMessenger);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create debug messenger callback! {0}", result);
	}
	uint32_t VulkanContext::EvaluateDevice(VkPhysicalDevice device)
	{
		uint32_t score = 0;
		//Evaluate Device type
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);
		if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 100;
		}
		else
		{
			score += 50;
		}

		//Check for required Queue Families
		VulkanUtility::QueueFamilyIndices queueFamilyIndices;
		VulkanUtility::QueryQueueFamilies(device, m_Surface, queueFamilyIndices);
		if (!queueFamilyIndices.isComplete())
		{
			score = 0;
			return score;
		}

		//Check for required extensions
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
		std::unordered_set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());
		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}
		if (!requiredExtensions.empty()) 
		{
			score = 0;
			return score;
		}

		//Verify swap chain support details
		VulkanUtility::SwapChainSupportDetails swapChainSupportDetails;
		VulkanUtility::QuerySwapChainSupport(device, m_Surface, swapChainSupportDetails);
		if (swapChainSupportDetails.surfaceFormats.empty() || swapChainSupportDetails.presentModes.empty())
		{
			score = 0;
			return score;
		}
		for (const auto& surfaceFormat : swapChainSupportDetails.surfaceFormats)
		{
			if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
				surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				score += 10;
				break;
			}
		}
		for (const auto& presentMode : swapChainSupportDetails.presentModes)
		{
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				score += 10;
				break;
			}
		}

		return score;
	}

	void VulkanContext::SelectPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		VkPhysicalDevice selectedDevice = VK_NULL_HANDLE;
		uint32_t selectedDeviceScore = 0;
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
		HZ_CORE_ASSERT(deviceCount > 0, "Failed to find a GPU with Vulkan support!");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			uint32_t deviceScore = EvaluateDevice(device);
			if (deviceScore > selectedDeviceScore)
			{
				selectedDevice = device;
				selectedDeviceScore = deviceScore;
			}
		}

		HZ_CORE_ASSERT(selectedDevice != VK_NULL_HANDLE, "Unable to find a suitable GPU!");
		m_PhysicalDevice = selectedDevice;
	}

	void VulkanContext::CreateLogicalDevice()
	{
		VkResult result;
		VulkanUtility::QueueFamilyIndices queueFamilyIndices;
		VulkanUtility::QueryQueueFamilies(m_PhysicalDevice, m_Surface, queueFamilyIndices);

		std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
		std::unordered_set<uint32_t> uniqueDeviceQueueFamilies = { queueFamilyIndices.graphicsFamily.value(),
			queueFamilyIndices.presentFamily.value() };
		float queuePriority = 1.0f;

		for (uint32_t deviceQueueFamily : uniqueDeviceQueueFamilies)
		{
			VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
			deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			deviceQueueCreateInfo.pNext = NULL;
			deviceQueueCreateInfo.flags = 0;
			deviceQueueCreateInfo.queueFamilyIndex = deviceQueueFamily;
			deviceQueueCreateInfo.queueCount = 1;
			deviceQueueCreateInfo.pQueuePriorities = &queuePriority;

			deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
		}

		VkPhysicalDeviceFeatures physicalDeviceFeatures = {};

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = NULL;
		deviceCreateInfo.flags = 0;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
		if (m_EnableValidationLayers)
		{
			deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
			deviceCreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();
		}
		else
		{
			deviceCreateInfo.enabledLayerCount = 0;
			deviceCreateInfo.ppEnabledLayerNames = NULL;
		}
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();
		deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

		result = vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_LogicalDevice);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create logical deivce! " + result);

		vkGetDeviceQueue(m_LogicalDevice, queueFamilyIndices.graphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_LogicalDevice, queueFamilyIndices.presentFamily.value(), 0, &m_PresentQueue);
	}

	void VulkanContext::CreateSurface()
	{
		VkResult result;
		result = glfwCreateWindowSurface(m_Instance, m_WindowHandle, nullptr, &m_Surface);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create window surface! " + result);
	}

	void VulkanContext::CreateCommandPool()
	{
		VkResult result;
		VulkanUtility::QueueFamilyIndices queueFamilyIndices;
		VulkanUtility::QueryQueueFamilies(m_PhysicalDevice, m_Surface, queueFamilyIndices);

		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.pNext = NULL;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		result = vkCreateCommandPool(m_LogicalDevice, &commandPoolCreateInfo, nullptr, &m_CommandPool);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create command pool! " + result);
	}

}