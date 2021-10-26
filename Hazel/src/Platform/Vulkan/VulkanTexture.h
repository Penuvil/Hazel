#pragma once

#include "Hazel/Renderer/Texture.h"
#include "Platform/Vulkan/VulkanContext.h"

#include <vulkan/vulkan.h>
#include <examples/imgui_impl_vulkan_with_textures.h>

namespace Hazel {

	class VulkanTexture2D : public Texture2D
	{
	public:
		struct TextureArrayDescriptorsSets
		{
			int descriptorCount = 0;
			std::vector<VkDescriptorSet> descriptorSets;
			std::array<VkDescriptorImageInfo, 32> descriptorImageInfos;

			void UpdateDescriptorSets(uint32_t swapImageIndex)
			{
				VkWriteDescriptorSet writeDescriptorSet = {};
				writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeDescriptorSet.pNext = 0;
				writeDescriptorSet.dstSet = descriptorSets[swapImageIndex];
				writeDescriptorSet.dstBinding = 2;
				writeDescriptorSet.dstArrayElement = 0;
				writeDescriptorSet.descriptorCount = descriptorImageInfos.size();
				writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				writeDescriptorSet.pImageInfo = descriptorImageInfos.data();
				writeDescriptorSet.pBufferInfo = nullptr;
				writeDescriptorSet.pTexelBufferView = nullptr;

				vkUpdateDescriptorSets(*VulkanContext::GetContext()->GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
			}
		};

		static TextureArrayDescriptorsSets s_TextureArrayDescriptorSets;

		VulkanTexture2D(uint32_t width, uint32_t height);
		VulkanTexture2D(const std::string& path);
		virtual ~VulkanTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual void* GetRendererID() override;

		virtual void SetData(void* data, uint32_t size) override;

		void CreateDescriptorSets();
		void UpdateDescriptorSets(uint32_t swapImageIndex) const;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual bool operator==(const Texture& other) const override
		{
			return m_Image == ((VulkanTexture2D&)other).m_Image;
		}

	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		VkImage m_Image;
		VkDeviceMemory m_Memory;
		VkImageView m_ImageView;
		VkSampler m_Sampler;
		VkFormat m_Format;
		VkDescriptorImageInfo m_ImageInfo;
		ImTextureID m_ImguiDescriptorSet;
	};
}