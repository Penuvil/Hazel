#include "hzpch.h"
#include <cstdint>

#include "Platform/Vulkan/VulkanSwapChain.h"

namespace Hazel
{
	VulkanSwapChain::VulkanSwapChain(GLFWwindow* windowHandle, VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface)
		:m_WindowHandle(windowHandle), m_PhysicalDevice(physicalDevice), m_Device(device), m_Surface(surface)
	{
		Init();
	}

	void VulkanSwapChain::Init()
	{
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateDescriptorPool();
	}

	void VulkanSwapChain::CreateSwapChain()
	{
		VulkanUtility::SwapChainSupportDetails swapChainSupportDetails;
		VulkanUtility::QuerySwapChainSupport(m_PhysicalDevice, m_Surface, swapChainSupportDetails);

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupportDetails.surfaceFormats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupportDetails.presentModes);
		VkExtent2D extent = ChooseSwapExtent(swapChainSupportDetails.surfaceCapabilities);

		uint32_t imageCount = swapChainSupportDetails.surfaceCapabilities.minImageCount + 1;
		if (swapChainSupportDetails.surfaceCapabilities.maxImageCount > 0 &&
			imageCount > swapChainSupportDetails.surfaceCapabilities.maxImageCount)
		{
			imageCount = swapChainSupportDetails.surfaceCapabilities.maxImageCount;
		}

		
		VulkanUtility::QueryQueueFamilies(m_PhysicalDevice, m_Surface, m_QueueFamilyIndices);
		uint32_t indices[] = { m_QueueFamilyIndices.graphicsFamily.value(), m_QueueFamilyIndices.presentFamily.value() };

		VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
		swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainCreateInfo.pNext = NULL;
		swapChainCreateInfo.flags = 0;
		swapChainCreateInfo.surface = m_Surface;
		swapChainCreateInfo.minImageCount = imageCount;
		swapChainCreateInfo.imageFormat = surfaceFormat.format;
		swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapChainCreateInfo.imageExtent = extent;
		swapChainCreateInfo.imageArrayLayers = 1;
		swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if (m_QueueFamilyIndices.graphicsFamily.value() != m_QueueFamilyIndices.presentFamily.value())
		{
			swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapChainCreateInfo.queueFamilyIndexCount = 2;
			swapChainCreateInfo.pQueueFamilyIndices = indices;
		}
		else
		{
			swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapChainCreateInfo.queueFamilyIndexCount = 0;
			swapChainCreateInfo.pQueueFamilyIndices = nullptr;
		}
		swapChainCreateInfo.preTransform = swapChainSupportDetails.surfaceCapabilities.currentTransform;
		swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapChainCreateInfo.presentMode = presentMode;
		swapChainCreateInfo.clipped = VK_TRUE;
		swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		VkResult result = vkCreateSwapchainKHR(m_Device, &swapChainCreateInfo, nullptr, &m_SwapChain);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create swap chain! {0}", result);

		vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, m_SwapChainImages.data());

		m_SwapChainImageFormat = surfaceFormat.format;
		m_SwapChainExtent = extent;
	}

	void VulkanSwapChain::CreateImageViews()
	{
		VkResult result;
		m_SwapChainImageViews.resize(m_SwapChainImages.size());

		for (size_t i = 0; i < m_SwapChainImages.size(); i++)
		{
			VkImageViewCreateInfo imageViewCreateInfo = {};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.pNext = NULL;
			imageViewCreateInfo.flags = 0;
			imageViewCreateInfo.image = m_SwapChainImages[i];
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = m_SwapChainImageFormat;
			imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;

			result = vkCreateImageView(m_Device, &imageViewCreateInfo, nullptr, &m_SwapChainImageViews[i]);
			HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create image views! {0}", result);
		}
	}

	void VulkanSwapChain::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.flags = 0;
		colorAttachment.format = m_SwapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.flags = 0;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentRef;
		subpassDescription.pResolveAttachments = nullptr;
		subpassDescription.pDepthStencilAttachment = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;

		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = NULL;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &colorAttachment;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.dependencyCount = 0;
		renderPassCreateInfo.pDependencies = nullptr;

		VkResult result = vkCreateRenderPass(m_Device, &renderPassCreateInfo, nullptr, &m_RenderPass);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create render pass! " + result);
	}

	void VulkanSwapChain::CreateDescriptorPool()
	{
		VkDescriptorPoolSize descriptorPoolSize = {};
		descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorPoolSize.descriptorCount = static_cast<uint32_t>(m_SwapChainImages.size());

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.pNext = NULL;
		descriptorPoolCreateInfo.flags = 0;
		descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(m_SwapChainImages.size());
		descriptorPoolCreateInfo.poolSizeCount = 1;
		descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;

		VkResult result = vkCreateDescriptorPool(m_Device, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create descriptor pool! " + result);
	}

	VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& supportedFormats)
	{
		for (const auto& format : supportedFormats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
				format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
			{
				return format;
			}
		}
		return supportedFormats[0];
	}

	VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& supportedPresentModes)
	{
		for (const auto& presentMode : supportedPresentModes)
		{
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
			{
				return presentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}
		else
		{
			int width = 0;
			int height = 0;
			glfwGetWindowSize(m_WindowHandle, &width, &height);
			VkExtent2D actualExtent = { width, height };

			actualExtent.width = std::max(capabilities.minImageExtent.width, 
				std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height,
				std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	void VulkanSwapChain::Destroy()
	{
		vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
		vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);

		for (auto imageview : m_SwapChainImageViews) 
		{
			vkDestroyImageView(m_Device, imageview, nullptr);
		}

		vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
	}
}