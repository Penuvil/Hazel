#include "hzpch.h"

#include "Platform/Vulkan/VulkanUtility.h"

namespace Hazel {

	void VulkanUtility::QueryQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface, QueueFamilyIndices& indices)
	{
		uint32_t queueFamilyCount = 0;
		//		QueueFamilyIndices indices;
		VkBool32 presentationSupport = false;

		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				if (!indices.graphicsFamily.has_value())
				{
					indices.graphicsFamily = i;
				}
			}

			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationSupport);
			if (queueFamily.queueCount > 0 && presentationSupport)
			{
				if (!indices.presentFamily.has_value())
				{
					indices.presentFamily = i;
				}
			}

			if (indices.isComplete())
			{
				break;
			}

			i++;
		}

		return;
	}

	void VulkanUtility::QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface, SwapChainSupportDetails& details)
	{
		//		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.surfaceCapabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
		if (formatCount != 0)
		{
			details.surfaceFormats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.surfaceFormats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());

		}
		return;
	}
}