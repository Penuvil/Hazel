#pragma once

#include "Hazel/Renderer/Buffer.h"

#include <vulkan/vulkan.h>

namespace Hazel {

	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(float* vertices, uint32_t size);
		virtual ~VulkanVertexBuffer();

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
}
