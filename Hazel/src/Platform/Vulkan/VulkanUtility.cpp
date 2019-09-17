#include "hzpch.h"

#include "Platform/Vulkan/VulkanUtility.h"
#include "Platform/Vulkan/VulkanContext.h"

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

	uint32_t VulkanUtility::FindMemoryTypeIndex(uint32_t typeRequirement, VkMemoryPropertyFlags propertyFlags)
	{
		VkPhysicalDevice* physicalDevice = VulkanContext::GetContext().GetPhysicalDevice();
		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
		vkGetPhysicalDeviceMemoryProperties(*physicalDevice, &physicalDeviceMemoryProperties);

		for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++)
		{
			if (typeRequirement & (1 << i) &&
				(physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags)
			{
				return i;
			}
		}
		HZ_CORE_ASSERT(false, "Failed to find suitable memory type!");
	}

	void VulkanUtility::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags propertyFlags, VkBuffer & buffer, VkDeviceMemory & bufferMemory)
	{
		VkResult result;
		VkDevice* device = VulkanContext::GetContext().GetDevice();
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = NULL;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = bufferUsage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;

		result = vkCreateBuffer(*device, &bufferCreateInfo, nullptr, &buffer);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create vertex buffer!");

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(*device, buffer, &memoryRequirements);
		

		uint32_t memoryTypeIndex = UINT32_MAX;
		memoryTypeIndex = VulkanUtility::FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, propertyFlags);

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = NULL;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

		result = vkAllocateMemory(*device, &memoryAllocateInfo, nullptr, &bufferMemory);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate buffer memory!");

		result = vkBindBufferMemory(*device, buffer, bufferMemory, 0);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to bind buffer memory!");
	}

	void VulkanUtility::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags propertyFlags, std::vector<VkBuffer> & buffers, VkDeviceMemory & bufferMemory)
	{
		VkResult result;
		VkDevice* device = VulkanContext::GetContext().GetDevice();
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = NULL;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = bufferUsage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;

		for (auto& buffer : buffers) 
		{
			result = vkCreateBuffer(*device, &bufferCreateInfo, nullptr, &buffer);
			HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create vertex buffer!");
		}

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(*device, buffers[0], &memoryRequirements);


		uint32_t memoryTypeIndex = UINT32_MAX;
		memoryTypeIndex = VulkanUtility::FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, propertyFlags);

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.pNext = NULL;
		memoryAllocateInfo.allocationSize = memoryRequirements.size * buffers.size();
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

		result = vkAllocateMemory(*device, &memoryAllocateInfo, nullptr, &bufferMemory);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate buffer memory!");
		
		for (int i = 0; i < buffers.size(); i++)
		{
			result = vkBindBufferMemory(*device, buffers[i], bufferMemory, i * size);
			HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to bind buffer memory!");
		}
	}

	void VulkanUtility::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{ 
		VkResult result;
		VkDevice* device = VulkanContext::GetContext().GetDevice();
		VkCommandPool* commandPool = VulkanContext::GetContext().GetCommandPool();
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = NULL;
		commandBufferAllocateInfo.commandPool = *commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		result = vkAllocateCommandBuffers(*device, &commandBufferAllocateInfo, &commandBuffer);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate command buffers! {0}", result);

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = NULL;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		commandBufferBeginInfo.pInheritanceInfo = VK_NULL_HANDLE;

		result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to begin command buffer! {0}", result);

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;

		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		result = vkEndCommandBuffer(commandBuffer);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to end command buffer! {0}", result);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = NULL;
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;

		VkQueue* graphicsQueue = VulkanContext::GetContext().GetGraphicsQueue();
		result = vkQueueSubmit(*graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to submit command buffers to queue! {0}", result);
		vkQueueWaitIdle(*graphicsQueue);

		vkFreeCommandBuffers(*device, *commandPool, 1, &commandBuffer);
 	}
}