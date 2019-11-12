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
		CreateDepthResources();
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
			m_SwapChainImageViews[i] = VulkanUtility::CreateImageView(m_SwapChainImages[i], m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
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

		VkAttachmentDescription depthAttachment = {};
		depthAttachment.flags = 0;
		depthAttachment.format = m_DepthImageFormat;
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.flags = 0;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentRef;
		subpassDescription.pResolveAttachments = nullptr;
		subpassDescription.pDepthStencilAttachment = &depthAttachmentRef;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;

		std::array<VkSubpassDependency, 2> dependencies;

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = NULL;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassCreateInfo.pAttachments = attachments.data();
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassCreateInfo.pDependencies = dependencies.data();

		VkResult result = vkCreateRenderPass(m_Device, &renderPassCreateInfo, nullptr, &m_RenderPasses["Clear"]);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create render pass! " + result);
				
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		attachments = { colorAttachment, depthAttachment };
		
		result = vkCreateRenderPass(m_Device, &renderPassCreateInfo, nullptr, &m_RenderPasses["NoClear"]);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create render pass! " + result);
	}

	void VulkanSwapChain::CreateDepthResources()
	{
		m_DepthImageFormat = VulkanUtility::FindSupportedFormat({ VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

		VulkanUtility::CreateImage(m_SwapChainExtent.width, m_SwapChainExtent.height, m_DepthImageFormat, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_DepthImage, m_DepthImageMemory);

		m_DepthImageView = VulkanUtility::CreateImageView(m_DepthImage, m_DepthImageFormat, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
		VulkanUtility::TransitionImageLayout(m_DepthImage, m_DepthImageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}

	void VulkanSwapChain::CreateFramebuffers()
	{
		m_Framebuffers.resize(m_SwapChainImageViews.size());

		for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
		{
			VkResult result;
			std::array<VkImageView, 2> attachments = { m_SwapChainImageViews[i], m_DepthImageView };

			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.pNext = NULL;
			framebufferCreateInfo.flags = 0;
			framebufferCreateInfo.renderPass = m_RenderPasses["Clear"];
			framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferCreateInfo.pAttachments = attachments.data();
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
		descriptorPoolSizes[0].descriptorCount = static_cast<uint32_t>(m_SwapChainImages.size() * 2 * 500);

		descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorPoolSizes[1].descriptorCount = static_cast<uint32_t>(m_SwapChainImages.size() * 405);

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.pNext = NULL;
		descriptorPoolCreateInfo.flags = 0;
		descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(m_SwapChainImages.size() * 500);
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

		std::vector<VkDescriptorSetLayoutBinding> uniformDescriptorSetLayoutBindings = { matricesUboLayoutBinding, colorUboLayoutBinding };
		std::vector<VkDescriptorSetLayoutBinding> textureDescriptorSetLayoutBindings = { textureLayoutBinding };

		VkDescriptorSetLayout uniformDescriptorSetLayout;
		VkDescriptorSetLayout textureDescriptorSetLayout;

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = NULL;
		descriptorSetLayoutCreateInfo.flags = 0;
		descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(uniformDescriptorSetLayoutBindings.size());
		descriptorSetLayoutCreateInfo.pBindings = uniformDescriptorSetLayoutBindings.data();

		VkResult result;
		result = vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &uniformDescriptorSetLayout);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create descriptor set layout! " + result);

		descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(textureDescriptorSetLayoutBindings.size());
		descriptorSetLayoutCreateInfo.pBindings = textureDescriptorSetLayoutBindings.data();

		result = vkCreateDescriptorSetLayout(m_Device, &descriptorSetLayoutCreateInfo, nullptr, &textureDescriptorSetLayout);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create descriptor set layout! " + result);

		m_DescriptorSetLayouts.reserve(2);
		m_DescriptorSetLayouts.push_back(uniformDescriptorSetLayout);
		m_DescriptorSetLayouts.push_back(textureDescriptorSetLayout);
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

		for (auto layout : m_DescriptorSetLayouts) 
		{
			vkDestroyDescriptorSetLayout(m_Device, layout, nullptr);
		}

		vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);

		for (auto framebuffer : m_Framebuffers)
		{
			vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
		}

		for (auto renderPass : m_RenderPasses)
		{
			vkDestroyRenderPass(m_Device, renderPass.second, nullptr);
		}

		vkDestroyImageView(m_Device, m_DepthImageView, nullptr);
		vkDestroyImage(m_Device, m_DepthImage, nullptr);
		vkFreeMemory(m_Device, m_DepthImageMemory, nullptr);

		for (auto imageview : m_SwapChainImageViews) 
		{
			vkDestroyImageView(m_Device, imageview, nullptr);
		}

		vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
	}

	void VulkanSwapChain::RecreateSwapChain()
	{
		uint32_t currentImageCount = static_cast<uint32_t>(m_SwapChainImages.size());

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

		vkDestroyImageView(m_Device, m_DepthImageView, nullptr);
		vkDestroyImage(m_Device, m_DepthImage, nullptr);
		vkFreeMemory(m_Device, m_DepthImageMemory, nullptr);

		for (auto imageview : m_SwapChainImageViews)
		{
			vkDestroyImageView(m_Device, imageview, nullptr);
		}

		vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);

		CreateSwapChain();
		CreateImageViews();
		CreateDepthResources();
		CreateRenderPass();
		CreateFramebuffers();

		HZ_CORE_ASSERT(currentImageCount == m_SwapChainImages.size(), "Unsupported change in swap change image count!");
	}
}