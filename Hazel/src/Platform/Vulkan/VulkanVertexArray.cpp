#include "hzpch.h"

#include "VulkanVertexArray.h"
#include "Platform/Vulkan/VulkanContext.h"
#include "Hazel/Renderer/Shader.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanBuffer.h"

namespace Hazel {

	VulkanVertexArray::VulkanVertexArray(uint32_t& numberOfInstances)
	{
		m_UniformBuffers.resize(numberOfInstances);
		m_DecsriptorSets.resize(numberOfInstances);

		for (uint32_t i = 0; i < numberOfInstances; i++) {
			std::unordered_map<std::string, Ref<UniformBuffer>> buffers;
			Ref<UniformBuffer> uniformBuffer;
			uniformBuffer.reset(VulkanUniformBuffer::Create("Matrices", 2 * ShaderDataTypeSize(ShaderDataType::Mat4), 0));
			m_UniformBuffers.at(i).insert({ uniformBuffer->GetName(), uniformBuffer });
			uniformBuffer.reset(VulkanUniformBuffer::Create("Color", ShaderDataTypeSize(ShaderDataType::Float3), 0));
			m_UniformBuffers.at(i).insert({ uniformBuffer->GetName(), uniformBuffer });

			CreateDescriptorSets(i);
		}
	}

	VulkanVertexArray::~VulkanVertexArray()
	{
	}

	void VulkanVertexArray::CreateDescriptorSets(uint32_t instanceIndex)
	{
		VkResult result;
		VulkanContext* context = VulkanContext::GetContext();
		Ref<VulkanSwapChain> swapChain = context->GetSwapChain();
		uint32_t swapImageCount = swapChain->GetImageCount();
		std::vector<VkBuffer>* matricesBuffers = std::static_pointer_cast<VulkanUniformBuffer>(m_UniformBuffers[instanceIndex].find("Matrices")->second)->GetBuffers();
		std::vector<VkBuffer>* colorBuffers = std::static_pointer_cast<VulkanUniformBuffer>(m_UniformBuffers[instanceIndex].find("Color")->second)->GetBuffers();;
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts(swapImageCount, *swapChain->GetDescriptorSetLayout());

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.pNext = NULL;
		descriptorSetAllocateInfo.descriptorPool = *swapChain->GetDescriptorPool();
		descriptorSetAllocateInfo.descriptorSetCount = swapImageCount;
		descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

		m_DecsriptorSets[instanceIndex].resize(swapImageCount);

		result = vkAllocateDescriptorSets(*context->GetDevice(), &descriptorSetAllocateInfo, m_DecsriptorSets[instanceIndex].data());
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate descriptor sets! ");

		for (uint32_t i = 0; i < swapImageCount; i++)
		{
			VkDescriptorBufferInfo matricesDescriptorBufferInfo = {};
			matricesDescriptorBufferInfo.buffer = matricesBuffers->at(i);
			matricesDescriptorBufferInfo.offset = 0;
			matricesDescriptorBufferInfo.range = ShaderDataTypeSize(ShaderDataType::Mat4) * 2;

			VkWriteDescriptorSet matricesWriteDescriptorSet = {};
			matricesWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			matricesWriteDescriptorSet.pNext = NULL;
			matricesWriteDescriptorSet.dstSet = m_DecsriptorSets[instanceIndex][i];
			matricesWriteDescriptorSet.dstBinding = 0;
			matricesWriteDescriptorSet.dstArrayElement = 0;
			matricesWriteDescriptorSet.descriptorCount = 1;
			matricesWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			matricesWriteDescriptorSet.pImageInfo = nullptr;
			matricesWriteDescriptorSet.pBufferInfo = &matricesDescriptorBufferInfo;
			matricesWriteDescriptorSet.pTexelBufferView = nullptr;

			VkDescriptorBufferInfo colorDescriptorBufferInfo = {};
			colorDescriptorBufferInfo.buffer = colorBuffers->at(i);
			colorDescriptorBufferInfo.offset = 0;
			colorDescriptorBufferInfo.range = ShaderDataTypeSize(ShaderDataType::Float3);

			VkWriteDescriptorSet colorWriteDescriptorSet = {};
			colorWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			colorWriteDescriptorSet.pNext = NULL;
			colorWriteDescriptorSet.dstSet = m_DecsriptorSets[instanceIndex][i];
			colorWriteDescriptorSet.dstBinding = 1;
			colorWriteDescriptorSet.dstArrayElement = 0;
			colorWriteDescriptorSet.descriptorCount = 1;
			colorWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			colorWriteDescriptorSet.pImageInfo = nullptr;
			colorWriteDescriptorSet.pBufferInfo = &colorDescriptorBufferInfo;
			colorWriteDescriptorSet.pTexelBufferView = nullptr;

			std::array<VkWriteDescriptorSet, 2> writeDescriptorSets[] = { matricesWriteDescriptorSet, colorWriteDescriptorSet };

			vkUpdateDescriptorSets(*context->GetDevice(), static_cast<uint32_t>(writeDescriptorSets->size()), writeDescriptorSets->data(), 0, nullptr);
		}

	}

	void VulkanVertexArray::Bind() const
	{
	}

	void VulkanVertexArray::Unbind() const
	{
	}

	void VulkanVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		HZ_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		const auto& layout = vertexBuffer->GetLayout();
		
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void VulkanVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		m_IndexBuffer = indexBuffer;
	}
}