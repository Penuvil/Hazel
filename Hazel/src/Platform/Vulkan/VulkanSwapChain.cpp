#include "hzpch.h"
#include <cstdint>

#include "Platform/Vulkan/VulkanSwapChain.h"
#include "Platform/Vulkan/VulkanContext.h"

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
		CreateFramebuffers();
		CreateDescriptorPool();
		CreateDescriptorSetLayout();
		AllocateCommandBuffers();
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
		m_SwapChainImageViews.resize(m_SwapChainImages.size());

		for (size_t i = 0; i < m_SwapChainImages.size(); i++)
		{
			m_SwapChainImageViews[i] = VulkanUtility::CreateImageView(m_SwapChainImages[i], m_SwapChainImageFormat);
		}
	}

	void VulkanSwapChain::CreateRenderPass()
	{
		m_RenderPasses.reserve(2);

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

		VkSubpassDependency subpassDependency = {};
		subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependency.dstSubpass = 0;
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.srcAccessMask = 0;
		subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpassDependency.dependencyFlags = 0;

		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = NULL;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &colorAttachment;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &subpassDependency;

		VkResult result = vkCreateRenderPass(m_Device, &renderPassCreateInfo, nullptr, &m_RenderPasses["Clear"]);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create render pass! " + result);

		colorAttachment = {};
		colorAttachment.flags = 0;
		colorAttachment.format = m_SwapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		result = vkCreateRenderPass(m_Device, &renderPassCreateInfo, nullptr, &m_RenderPasses["NoClear"]);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create render pass! " + result);
	}

	void VulkanSwapChain::CreateFramebuffers()
	{
		m_Framebuffers.resize(m_SwapChainImageViews.size());

		for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
		{
			VkResult result;
			VkImageView attachment = m_SwapChainImageViews[i];

			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.pNext = NULL;
			framebufferCreateInfo.flags = 0;
			framebufferCreateInfo.renderPass = m_RenderPasses["Clear"];
			framebufferCreateInfo.attachmentCount = 1;
			framebufferCreateInfo.pAttachments = &attachment;
			framebufferCreateInfo.width = m_SwapChainExtent.width;
			framebufferCreateInfo.height = m_SwapChainExtent.height;
			framebufferCreateInfo.layers = 1;

			result = vkCreateFramebuffer(m_Device, &framebufferCreateInfo, nullptr, &m_Framebuffers[i]);
			HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create framebuffer")
		}
	}

	void VulkanSwapChain::CreateDescriptorPool()
	{
		std::array<VkDescriptorPoolSize, 2> descriptorPoolSizes = {};
		descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorPoolSizes[0].descriptorCount = static_cast<uint32_t>(m_SwapChainImages.size() * 2 * 404);

		descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorPoolSizes[1].descriptorCount = static_cast<uint32_t>(m_SwapChainImages.size() * 404);

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.pNext = NULL;
		descriptorPoolCreateInfo.flags = 0;
		descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(m_SwapChainImages.size() * 2 * 404);
		descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
		descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

		VkResult result = vkCreateDescriptorPool(m_Device, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create descriptor pool! " + result);
	}

	void VulkanSwapChain::CreateDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding matricesUboLayoutBinding = {};
		matricesUboLayoutBinding.binding = 0;
		matricesUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		matricesUboLayoutBinding.descriptorCount = 1;
		matricesUboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		matricesUboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding colorUboLayoutBinding = {};
		colorUboLayoutBinding.binding = 1;
		colorUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		colorUboLayoutBinding.descriptorCount = 1;
		colorUboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		colorUboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding textureLayoutBinding = {};
		textureLayoutBinding.binding = 2;
		textureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		textureLayoutBinding.descriptorCount = 1;
		textureLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		textureLayoutBinding.pImmutableSamplers = nullptr;

		std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = { matricesUboLayoutBinding, colorUboLayoutBinding, textureLayoutBinding };

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = NULL;
		descriptorSetLayoutCreateInfo.flags = 0;
		descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
		descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();

		VkResult result;
		result = vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &m_DescriptorSetLayout);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create descriptor set layout! " + result);
	}

	void VulkanSwapChain::AllocateCommandBuffers()
	{
		VkResult result;
		m_CommandBuffers.resize(m_Framebuffers.size());
		m_ImGuiCommandBuffers.resize(m_Framebuffers.size());

		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = NULL;
		commandBufferAllocateInfo.commandPool = *VulkanContext::GetContext()->GetCommandPool();
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

		result = vkAllocateCommandBuffers(m_Device, &commandBufferAllocateInfo, m_CommandBuffers.data());
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate commad buffers! " + result);

		result = vkAllocateCommandBuffers(m_Device, &commandBufferAllocateInfo, m_ImGuiCommandBuffers.data());
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate ImGui commad buffer! " + result);
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
			VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

			actualExtent.width = std::max(capabilities.minImageExtent.width, 
				std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height,
				std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	void VulkanSwapChain::Destroy()
	{

		vkDeviceWaitIdle(m_Device);
		vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);

		for (auto framebuffer : m_Framebuffers)
		{
			vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
		}

//		vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
		for (auto renderPass : m_RenderPasses)
		{
			vkDestroyRenderPass(m_Device, renderPass.second, nullptr);
		}

		for (auto imageview : m_SwapChainImageViews) 
		{
			vkDestroyImageView(m_Device, imageview, nullptr);
		}

		vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
	}

	void VulkanSwapChain::RecreateSwapChain()
	{
		uint32_t currentImageCount = m_SwapChainImages.size();

		vkDeviceWaitIdle(m_Device);
		for(auto framebuffer : m_Framebuffers)
		{
			vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
		}

		for (auto renderPass : m_RenderPasses)
		{
			vkDestroyRenderPass(m_Device, renderPass.second, nullptr);
		}
		m_RenderPasses.clear();

		for (auto imageview : m_SwapChainImageViews)
		{
			vkDestroyImageView(m_Device, imageview, nullptr);
		}

		vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);

		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateFramebuffers();

		HZ_CORE_ASSERT(currentImageCount == m_SwapChainImages.size(), "Unsupported change in swap change image count!");
	}
}