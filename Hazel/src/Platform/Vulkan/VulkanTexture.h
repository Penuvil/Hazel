#pragma once

#include "Hazel/Renderer/Texture.h"

#include <vulkan/vulkan.h>

namespace Hazel {

	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(const std::string& path);
		virtual ~VulkanTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual void Bind(uint32_t slot = 0) const override;
		void Bind(VkDescriptorSet descriptorSet) const;

	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		VkImage m_Image;
		VkDeviceMemory m_Memory;
		VkImageView m_ImageView;
		VkSampler m_Sampler;
	};
}