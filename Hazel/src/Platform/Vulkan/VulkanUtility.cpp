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
		VkPhysicalDevice* physicalDevice = VulkanContext::GetContext()->GetPhysicalDevice();
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

	void VulkanUtility::CreateBuffer(VkDeviceSize& size, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags propertyFlags, VkBuffer & buffer, VkDeviceMemory & bufferMemory)
	{
		VkResult result;
		VkDevice* device = VulkanContext::GetContext()->GetDevice();
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

		if (memoryRequirements.size > size) size = memoryRequirements.size;
		
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

	void VulkanUtility::CreateBuffer(VkDeviceSize& size, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags propertyFlags, std::vector<VkBuffer> & buffers, VkDeviceMemory & bufferMemory)
	{
		VkResult result;
		VkDevice* device = VulkanContext::GetContext()->GetDevice();
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext = NULL;
		bufferCreateInfo.flags = 0;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = bufferUsage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount = 0;
		bufferCreateInfo.pQueueFamilyIndices = nullptr;


		VkMemoryRequirements memoryRequirements;
		
		for (auto& buffer : buffers) 
		{
			result = vkCreateBuffer(*device, &bufferCreateInfo, nullptr, &buffer);
			HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create vertex buffer!");

			vkGetBufferMemoryRequirements(*device, buffer, &memoryRequirements);
		}

//		VkMemoryRequirements memoryRequirements;
//		vkGetBufferMemoryRequirements(*device, buffers[0], &memoryRequirements);

		if (memoryRequirements.size > size) size = memoryRequirements.size;

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

	VkCommandBuffer VulkanUtility::BeginTransientCommand() {
		VkResult result;
		VkDevice* device = VulkanContext::GetContext()->GetDevice();
		VkCommandPool* commandPool = VulkanContext::GetContext()->GetCommandPool();

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

		return commandBuffer;
	}

	void VulkanUtility::EndTransientCommand(VkCommandBuffer commandBuffer)
	{
		VkResult result;
		VkDevice* device = VulkanContext::GetContext()->GetDevice();
		VkCommandPool* commandPool = VulkanContext::GetContext()->GetCommandPool();

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

		VkQueue* graphicsQueue = VulkanContext::GetContext()->GetGraphicsQueue();
		result = vkQueueSubmit(*graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to submit command buffers to queue! {0}", result);
		vkQueueWaitIdle(*graphicsQueue);

		vkFreeCommandBuffers(*device, *commandPool, 1, &commandBuffer);
	}

	void VulkanUtility::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{ 
		VkCommandBuffer commandBuffer = BeginTransientCommand();

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;

		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		EndTransientCommand(commandBuffer);
 	}

	void VulkanUtility::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	{
		VkCommandBuffer commandBuffer = BeginTransientCommand();

		VkBufferImageCopy imageCopyRegion = {};
		imageCopyRegion.bufferOffset = 0;
		imageCopyRegion.bufferRowLength = 0;
		imageCopyRegion.bufferImageHeight = 0;
		imageCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageCopyRegion.imageSubresource.mipLevel = 0;
		imageCopyRegion.imageSubresource.baseArrayLayer = 0;
		imageCopyRegion.imageSubresource.layerCount = 1;
		imageCopyRegion.imageOffset = { 0, 0, 0 };
		imageCopyRegion.imageExtent = { width, height, 1 };

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyRegion);

		EndTransientCommand(commandBuffer);
	}

	void VulkanUtility::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
		VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& memory)
	{
		VkResult result;
		VkDevice* device = VulkanContext::GetContext()->GetDevice();

		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = NULL;
		imageCreateInfo.flags = 0;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = format;
		imageCreateInfo.extent.width = width;
		imageCreateInfo.extent.height = height;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = tiling;
		imageCreateInfo.usage = usage;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.queueFamilyIndexCount = 0;
		imageCreateInfo.pQueueFamilyIndices = nullptr;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		result = vkCreateImage(*device, &imageCreateInfo, nullptr, &image);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create image!");

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(*device, image, &memRequirements);

		VkMemoryAllocateInfo memAllocateInfo = {};
		memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAllocateInfo.pNext = NULL;
		memAllocateInfo.allocationSize = memRequirements.size;
		memAllocateInfo.memoryTypeIndex = VulkanUtility::FindMemoryTypeIndex(memRequirements.memoryTypeBits, properties);

		result = vkAllocateMemory(*device, &memAllocateInfo, nullptr, &memory);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate memory!");

		vkBindImageMemory(*device, image, memory, 0);
	}

	void VulkanUtility::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer commandBuffer = BeginTransientCommand();
		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		VkImageMemoryBarrier imageMemBarrier = {};
		imageMemBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemBarrier.pNext = NULL;
		imageMemBarrier.oldLayout = oldLayout;
		imageMemBarrier.newLayout = newLayout;
		imageMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemBarrier.image = image;
		imageMemBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageMemBarrier.subresourceRange.baseMipLevel = 0;
		imageMemBarrier.subresourceRange.levelCount = 1;
		imageMemBarrier.subresourceRange.baseArrayLayer = 0;
		imageMemBarrier.subresourceRange.layerCount = 1;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			imageMemBarrier.srcAccessMask = 0;
			imageMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			imageMemBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			HZ_CORE_ASSERT(0, "Unsupported layout transition!");
		}

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &imageMemBarrier);

		EndTransientCommand(commandBuffer);
	}

	VkImageView VulkanUtility::CreateImageView(VkImage image, VkFormat format)
	{
		VkResult result;
		VkImageView imageView;
		VkDevice* device = VulkanContext::GetContext()->GetDevice();

		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = NULL;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = image;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		result = vkCreateImageView(*device, &imageViewCreateInfo, nullptr, &imageView);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create image views! {0}", result);

		return imageView;
	}
}