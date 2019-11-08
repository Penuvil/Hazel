#include "hzpch.h"

#include "VulkanTexture.h"
#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanUtility.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Platform/Vulkan/VulkanShader.h"

#include "stb_image.h"


namespace Hazel {
	VulkanTexture2D::VulkanTexture2D(uint32_t width, uint32_t height)
		:m_Width(width), m_Height(height)
	{
		VkResult result;
		VulkanContext* vulkanContext = VulkanContext::GetContext();
		VkDevice* device = vulkanContext->GetDevice();

		m_Format = VK_FORMAT_R8G8B8A8_UNORM;

		VulkanUtility::CreateImage(m_Width, m_Height, m_Format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_Memory);

		m_ImageView = VulkanUtility::CreateImageView(m_Image, m_Format);

		VkSamplerCreateInfo samplerCreateInfo = {};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.pNext = NULL;
		samplerCreateInfo.flags = 0;
		samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.anisotropyEnable = VK_TRUE;
		samplerCreateInfo.maxAnisotropy = 16;
		samplerCreateInfo.compareEnable = VK_TRUE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = 0.0f;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

		result = vkCreateSampler(*device, &samplerCreateInfo, nullptr, &m_Sampler);

		CreateDescriptorSets();
	}

	VulkanTexture2D::VulkanTexture2D(const std::string & path)
		: m_Path(path)
	{
		VulkanContext* vulkanContext = VulkanContext::GetContext();
		VkPhysicalDevice* physicalDevice = vulkanContext->GetPhysicalDevice();
		VkFormatProperties formatProperties;

		vkGetPhysicalDeviceFormatProperties(*physicalDevice, VK_FORMAT_R8G8B8_UNORM, &formatProperties);
		int32_t requiredChannels = (formatProperties.optimalTilingFeatures) ? 0 : 4;

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* imageData = stbi_load(path.c_str(), &width, &height, &channels, requiredChannels);
		HZ_CORE_ASSERT(imageData, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		VkDeviceSize imageSize = m_Width * m_Height * 4;
		VkBool32 anisotrophy;

		if (channels == 4)
		{
			m_Format = VK_FORMAT_R8G8B8A8_UNORM;
			anisotrophy = VK_TRUE;
		}
		else if (channels == 3)
		{
			
			m_Format = (requiredChannels == 0) ? VK_FORMAT_R8G8B8_UNORM : VK_FORMAT_R8G8B8A8_UNORM;
			anisotrophy = VK_FALSE;
		}

		VkResult result;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		VulkanUtility::CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		VkDevice* device = vulkanContext->GetDevice();
		void* data;
		vkMapMemory(*device, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, imageData, imageSize);
		vkUnmapMemory(*device, stagingBufferMemory);

		stbi_image_free(imageData);
		
		VulkanUtility::CreateImage(m_Width, m_Height, m_Format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_Memory);

		VulkanUtility::TransitionImageLayout(m_Image, m_Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		VulkanUtility::CopyBufferToImage(stagingBuffer, m_Image, m_Width, m_Height);
		VulkanUtility::TransitionImageLayout(m_Image, m_Format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(*device, stagingBuffer, nullptr);
		vkFreeMemory(*device, stagingBufferMemory, nullptr);

		m_ImageView = VulkanUtility::CreateImageView(m_Image, m_Format);

		VkSamplerCreateInfo samplerCreateInfo = {};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.pNext = NULL;
		samplerCreateInfo.flags = 0;
		samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.anisotropyEnable = anisotrophy;
		samplerCreateInfo.maxAnisotropy = 16;
		samplerCreateInfo.compareEnable = VK_TRUE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = 0.0f;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

		result = vkCreateSampler(*device, &samplerCreateInfo, nullptr, &m_Sampler);

		CreateDescriptorSets();
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
		VkDevice* device = VulkanContext::GetContext()->GetDevice();
		vkDestroySampler(*device, m_Sampler, nullptr);
		vkDestroyImageView(*device, m_ImageView, nullptr);
		vkDestroyImage(*device, m_Image, nullptr);
		vkFreeMemory(*device, m_Memory, nullptr);
	}

	void VulkanTexture2D::SetData(void * data, uint32_t size)
	{
		uint32_t bpp = m_Format == VK_FORMAT_R8G8B8A8_UNORM ? 4 : 3;
		HZ_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");

		VkResult result;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		VkDeviceSize imageSize = size;
		VulkanUtility::CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		VkDevice* device = VulkanContext::GetContext()->GetDevice();
		void* bufData;
		vkMapMemory(*device, stagingBufferMemory, 0, imageSize, 0, &bufData);
		memcpy(bufData, data, imageSize);
		vkUnmapMemory(*device, stagingBufferMemory);

		VulkanUtility::TransitionImageLayout(m_Image, m_Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		VulkanUtility::CopyBufferToImage(stagingBuffer, m_Image, m_Width, m_Height);
		VulkanUtility::TransitionImageLayout(m_Image, m_Format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(*device, stagingBuffer, nullptr);
		vkFreeMemory(*device, stagingBufferMemory, nullptr);
	}

	void VulkanTexture2D::CreateDescriptorSets()
	{
		VkResult result;
		VulkanContext* context = VulkanContext::GetContext();
		Ref<VulkanSwapChain> swapChain = context->GetSwapChain();
		uint32_t swapImageCount = swapChain->GetImageCount();
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts(swapImageCount, swapChain->GetDescriptorSetLayouts()->at(1));

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.pNext = NULL;
		descriptorSetAllocateInfo.descriptorPool = *swapChain->GetDescriptorPool();
		descriptorSetAllocateInfo.descriptorSetCount = swapImageCount;
		descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

		m_DescriptorSets.resize(swapImageCount);

		result = vkAllocateDescriptorSets(*context->GetDevice(), &descriptorSetAllocateInfo, m_DescriptorSets.data());
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate descriptor sets! ");

		for (uint32_t i = 0; i < swapImageCount; i++)
		{
			VkDescriptorImageInfo imageInfo = {};
			imageInfo.sampler = m_Sampler;
			imageInfo.imageView = m_ImageView;
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkWriteDescriptorSet writeDescriptorSet = {};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.pNext = 0;
			writeDescriptorSet.dstSet = m_DescriptorSets[i];
			writeDescriptorSet.dstBinding = 2;
			writeDescriptorSet.dstArrayElement = 0;
			writeDescriptorSet.descriptorCount = 1;
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writeDescriptorSet.pImageInfo = &imageInfo;
			writeDescriptorSet.pBufferInfo = nullptr;
			writeDescriptorSet.pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(*VulkanContext::GetContext()->GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
		}
	}

	void VulkanTexture2D::Bind(uint32_t slot) const
	{
		std::vector<VkCommandBuffer>* commandBuffers = VulkanContext::GetContext()->GetSwapChain()->GetCommandBuffers();

		vkCmdBindDescriptorSets(commandBuffers->at(VulkanRendererAPI::GetFrame()->imageIndex), VK_PIPELINE_BIND_POINT_GRAPHICS, *std::static_pointer_cast<VulkanShader>(VulkanRendererAPI::GetBatch()->shader)->GetGraphicsPipelineLayout(),
			1, 1, &GetDescriptorSets().at(VulkanRendererAPI::GetFrame()->imageIndex), 0, nullptr);
	}

	void VulkanTexture2D::Bind(VkDescriptorSet descriptorSet) const
	{
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.sampler = m_Sampler;
		imageInfo.imageView = m_ImageView;
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.pNext = 0;
		writeDescriptorSet.dstSet = descriptorSet;
		writeDescriptorSet.dstBinding = 2;
		writeDescriptorSet.dstArrayElement = 0;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSet.pImageInfo = &imageInfo;
		writeDescriptorSet.pBufferInfo = nullptr;
		writeDescriptorSet.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(*VulkanContext::GetContext()->GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
	}
}