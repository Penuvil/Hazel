#pragma once

#include "Hazel/Renderer/Buffer.h"

#include <vulkan/vulkan.h>

namespace Hazel {

	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(float* vertices, uint32_t size);
		virtual ~VulkanVertexBuffer();

		const inline VkBuffer* GetBuffer() { return &m_Buffer; }

		// Inherited via VertexBuffer
		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual const BufferLayout & GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout & layout) override;

	private:
		std::vector<float> m_Vertices;
		BufferLayout m_Layout;
		VkBuffer m_Buffer;
		VkDeviceMemory m_BufferMemory;		
	};

	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~VulkanIndexBuffer();

		const inline VkBuffer* GetBuffer() { return &m_Buffer; }

		// Inherited via IndexBuffer
		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual uint32_t GetCount() const { return m_Count; }
	private:
		uint32_t m_Count;
		std::vector<uint32_t> m_Indices;
		VkBuffer m_Buffer;
		VkDeviceMemory m_BufferMemory;
	};

	class VulkanUniformBuffer : public UniformBuffer
	{	
	public:
		VulkanUniformBuffer(std::string name, uint32_t size, uint32_t shaderBlock);
		virtual ~VulkanUniformBuffer();

		inline uint32_t* GetBufferSize() { return &m_BufferSize; }
		inline std::vector<VkBuffer>* GetBuffers() { return &m_Buffers; }
		inline VkDeviceMemory* GetBufferMemory() { return &m_BufferMemory; }
//		inline VkDescriptorSetLayout* GetDescriptorSetLayout() { return &m_DescriptorSetLayout; }
//		inline const std::vector<VkDescriptorSet>* GetDescriptorSets() { return &m_DecsriptorSets; }



		// Inherited via UniformBuffer
		virtual void Bind() override;
		virtual void Unbind() override;
		virtual std::string GetName() override;
	private:
		std::string m_Name;
		uint32_t m_BufferSize;
		std::vector<VkBuffer> m_Buffers;
		VkDeviceMemory m_BufferMemory;
//		VkDescriptorSetLayout m_DescriptorSetLayout;
//		std::vector<VkDescriptorSet> m_DecsriptorSets;
	};
}
