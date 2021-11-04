#pragma once

#include "Hazel/Renderer/Framebuffer.h"

#include <vulkan/vulkan.h>
#include <examples/imgui_impl_vulkan_with_textures.h>

namespace Hazel {

	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferSpecification& spec);
		virtual ~VulkanFramebuffer();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void* GetColorAttachmentRendererID() override;

		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		FramebufferSpecification m_Specification;
		std::vector<VkImage> m_ColorAttchments;
		std::vector<VkDeviceMemory> m_ColorAttachmentMemory;
		std::vector<VkImageView> m_ColorAttachemntImageViews;
		std::vector<ImTextureID> m_ImguiDescriptorSets;
		VkSampler m_Sampler;
		VkImage m_DepthAttachment;
		VkDeviceMemory m_DepthAttachemntMemory;
		VkImageView m_DepthAttachemntView;
		VkRenderPass m_RenderPass;
		std::vector<VkFramebuffer> m_Framebuffers;
	};
}