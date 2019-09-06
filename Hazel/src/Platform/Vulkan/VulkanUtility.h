#pragma once

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>


namespace Hazel
{
	class VulkanUtility
	{
	public:
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			bool isComplete()
			{
				return graphicsFamily.has_value() && presentFamily.has_value();
			}
		};
		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR surfaceCapabilities;
			std::vector<VkSurfaceFormatKHR> surfaceFormats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		static void QueryQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface, QueueFamilyIndices& indices);
		static void QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface, SwapChainSupportDetails& details);
	};
}