#include "hzpch.h"

#include "VulkanBuffer.h"
#include "VulkanContext.h"

namespace Hazel {

	/////////////////////////////////////////////////////////////////////////////
	// VertexBuffer /////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	VulkanVertexBuffer::VulkanVertexBuffer(float * vertices, uint32_t size)
	{
		m_Vertices.resize(size / sizeof(float));
		for (int i = 0; i < m_Vertices.size(); i++)
		{
			m_Vertices[i] = *(vertices + i);
		}
		VkDevice* device = VulkanContext::GetContext().GetDevice();
		VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		VulkanUtility::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(*device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, m_Vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(*device, stagingBufferMemory);

		VulkanUtility::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_BufferMemory);

		VulkanUtility::CopyBuffer(stagingBuffer, m_Buffer, bufferSize);

		vkDestroyBuffer(*device, stagingBuffer, nullptr);
		vkFreeMemory(*device, stagingBufferMemory, nullptr);

	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		VkDevice* device = VulkanContext::GetContext().GetDevice();
		vkDestroyBuffer(*device, m_Buffer, nullptr);
		vkFreeMemory(*device, m_BufferMemory, nullptr);
	}

	void VulkanVertexBuffer::Bind() const
	{
	}

	void VulkanVertexBuffer::Unbind() const
	{
	}

	void VulkanVertexBuffer::SetLayout(const BufferLayout & layout)
	{
		m_Layout = layout;
	}

	/////////////////////////////////////////////////////////////////////////////
	// IndexBuffer //////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t * indices, uint32_t count)
		:m_Count(count)
	{
		m_Indices.resize(m_Count);
		for (int i = 0; i < m_Indices.size(); i++)
		{
			m_Indices[i] = *(indices + i);
		}
		VkDevice* device = VulkanContext::GetContext().GetDevice();
		VkDeviceSize bufferSize = sizeof(m_Indices[0]) * m_Indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		VulkanUtility::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(*device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, m_Indices.data(), (size_t)bufferSize);
		vkUnmapMemory(*device, stagingBufferMemory);

		VulkanUtility::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_BufferMemory);

		VulkanUtility::CopyBuffer(stagingBuffer, m_Buffer, bufferSize);

		vkDestroyBuffer(*device, stagingBuffer, nullptr);
		vkFreeMemory(*device, stagingBufferMemory, nullptr);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		VkDevice* device = VulkanContext::GetContext().GetDevice();
		vkDestroyBuffer(*device, m_Buffer, nullptr);
		vkFreeMemory(*device, m_BufferMemory, nullptr);
	}

	void VulkanIndexBuffer::Bind() const
	{
	}

	void VulkanIndexBuffer::Unbind() const
	{
	}

	/////////////////////////////////////////////////////////////////////////////
	// UniformBuffer ////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	VulkanUniformBuffer::VulkanUniformBuffer(std::string name, uint32_t size, uint32_t shaderBlock)
	{
		m_Name = name;
		uint32_t swapImageCount = VulkanContext::GetContext().GetSwapChain()->GetImageCount();

		m_Buffers.resize(swapImageCount);

		VulkanUtility::CreateBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Buffers, m_BufferMemory);
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
	}

	void VulkanUniformBuffer::Bind()
	{
	}

	void VulkanUniformBuffer::Unbind()
	{
	}

	std::string VulkanUniformBuffer::GetName()
	{
		return m_Name;
	}

}