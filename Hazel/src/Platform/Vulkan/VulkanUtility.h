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
		static uint32_t FindMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags memoryProperties);
		static void CreateBuffer(VkDeviceSize& size, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags propertyFlags, VkBuffer& buffer, VkDeviceMemory &bufferMemory);
		static void CreateBuffer(VkDeviceSize& size, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags propertyFlags, std::vector<VkBuffer>& buffer, VkDeviceMemory & bufferMemory);
		static VkCommandBuffer BeginTransientCommand();
		static void EndTransientCommand(VkCommandBuffer commandBuffer);
		static void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		static void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		static void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage & image, VkDeviceMemory & memory);
		static void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout startingLayout, VkImageLayout newLayout);
		static VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		static VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	};
}