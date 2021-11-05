#include "hzpch.h"

#include "VulkanContext.h"
#include "VulkanRendererAPI.h"
#include "VulkanFramebuffer.h"
#include "VulkanUtility.h"

namespace Hazel {
	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		VulkanContext* context = VulkanContext::GetContext();
		VkDevice* device = context->GetDevice();
		uint32_t imageCount = context->GetSwapChain()->GetImageCount();
		VkFormat depthFormat;
		depthFormat = VulkanUtility::FindSupportedFormat({ VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

		m_ColorAttchments.resize(imageCount);
		m_ColorAttachmentMemory.resize(imageCount);
		m_ColorAttachemntImageViews.resize(imageCount);
		m_ImguiDescriptorSets.resize(imageCount);

		for (int i = 0; i < imageCount; i++)
		{
			VulkanUtility::CreateImage(m_Specification.Width, m_Specification.Height, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_ColorAttchments.at(i), m_ColorAttachmentMemory.at(i));

			m_ColorAttachemntImageViews.at(i) = VulkanUtility::CreateImageView(m_ColorAttchments.at(i), VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
		}

		VkSamplerCreateInfo samplerCreateInfo = {};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.pNext = NULL;
		samplerCreateInfo.flags = 0;
		samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.anisotropyEnable = VK_TRUE;
		samplerCreateInfo.maxAnisotropy = 1.0f;
		samplerCreateInfo.compareEnable = VK_TRUE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = 1.0f;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

		vkCreateSampler(*device, &samplerCreateInfo, nullptr, &m_Sampler);

		VulkanUtility::CreateImage(m_Specification.Width, m_Specification.Height, depthFormat, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_DepthAttachment, m_DepthAttachemntMemory);

		m_DepthAttachemntView = VulkanUtility::CreateImageView(m_DepthAttachment, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);

		std::array<VkAttachmentDescription, 2> attachmentDescriptions = {};

		attachmentDescriptions.at(0).flags = 0;
		attachmentDescriptions.at(0).format = VK_FORMAT_B8G8R8A8_UNORM;
		attachmentDescriptions.at(0).samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescriptions.at(0).loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescriptions.at(0).storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescriptions.at(0).stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescriptions.at(0).stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescriptions.at(0).initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDescriptions.at(0).finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		attachmentDescriptions.at(1).flags = 0;
		attachmentDescriptions.at(1).format = depthFormat;
		attachmentDescriptions.at(1).samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescriptions.at(1).loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescriptions.at(1).storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescriptions.at(1).stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescriptions.at(1).stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescriptions.at(1).initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDescriptions.at(1).finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachemntRef = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
		VkAttachmentReference depthAttachmentRef = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

		VkSubpassDescription subpassDescription = {};
		subpassDescription.flags = 0;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachemntRef;
		subpassDescription.pResolveAttachments = nullptr;
		subpassDescription.pDepthStencilAttachment = &depthAttachmentRef;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;

		std::array<VkSubpassDependency, 2> dependencies;

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = NULL;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
		renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassCreateInfo.pDependencies = dependencies.data();

		vkCreateRenderPass(*device, &renderPassCreateInfo, nullptr, &m_RenderPass);

		m_Framebuffers.resize(imageCount);

		for (int i = 0; i < imageCount; i++)
		{
			std::array<VkImageView, 2> attachments = { m_ColorAttachemntImageViews.at(i), m_DepthAttachemntView };

			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.pNext = NULL;
			framebufferCreateInfo.flags = 0;
			framebufferCreateInfo.renderPass = m_RenderPass;
			framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferCreateInfo.pAttachments = attachments.data();
			framebufferCreateInfo.width = m_Specification.Width;
			framebufferCreateInfo.height = m_Specification.Height;
			framebufferCreateInfo.layers = 1;

			vkCreateFramebuffer(*device, &framebufferCreateInfo, nullptr, &m_Framebuffers[i]);
		}

	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		VkDevice* device = VulkanContext::GetContext()->GetDevice();

		for (auto frameBuffer : m_Framebuffers)
			vkDestroyFramebuffer(*device, frameBuffer, nullptr);

		vkDestroyRenderPass(*device, m_RenderPass, nullptr);
		vkDestroySampler(*device, m_Sampler, nullptr);

		for (auto view : m_ColorAttachemntImageViews)
			vkDestroyImageView(*device, view, nullptr);

		for (auto image : m_ColorAttchments)
			vkDestroyImage(*device, image, nullptr);

		for (auto iMemory : m_ColorAttachmentMemory)
			vkFreeMemory(*device, iMemory, nullptr);

		vkDestroyImageView(*device, m_DepthAttachemntView, nullptr);
		vkDestroyImage(*device, m_DepthAttachment, nullptr);
		vkFreeMemory(*device, m_DepthAttachemntMemory, nullptr);
	}

	void VulkanFramebuffer::Bind()
	{
		auto currentFrame = VulkanRendererAPI::GetFrame();
		uint32_t imageIndex = currentFrame->imageIndex;
		VkCommandBuffer commandBuffer = VulkanRendererAPI::GetBatch()->commandBuffer;

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = NULL;
		renderPassBeginInfo.renderPass = m_RenderPass;
		renderPassBeginInfo.framebuffer = m_Framebuffers.at(imageIndex);
		renderPassBeginInfo.renderArea.offset = { 0,0 };
		renderPassBeginInfo.renderArea.extent.width = m_Specification.Width;
		renderPassBeginInfo.renderArea.extent.height = m_Specification.Height;
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(currentFrame->clearColors.size());
		renderPassBeginInfo.pClearValues = currentFrame->clearColors.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = (float)m_Specification.Height;
		viewport.width = (float)m_Specification.Width;
		viewport.height = -((float)m_Specification.Height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0 , 0 };
		scissor.extent.height = m_Specification.Height;
		scissor.extent.width = m_Specification.Width;

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}

	void VulkanFramebuffer::Unbind()
	{
		VkCommandBuffer commandBuffer = VulkanRendererAPI::GetBatch()->commandBuffer;
		vkCmdEndRenderPass(commandBuffer);
	}

	void* VulkanFramebuffer::GetColorAttachmentRendererID()
	{
		uint32_t imageIndex = VulkanRendererAPI::GetFrame()->imageIndex;

		if (m_ImguiDescriptorSets.at(imageIndex) == nullptr)
			m_ImguiDescriptorSets.at(imageIndex) = ImGui_ImplVulkan_AddTexture(m_Sampler, m_ColorAttachemntImageViews.at(imageIndex), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		return m_ImguiDescriptorSets.at(imageIndex);
	}
}