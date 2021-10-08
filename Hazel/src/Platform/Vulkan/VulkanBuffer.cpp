#include "hzpch.h"

#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Hazel {

	/////////////////////////////////////////////////////////////////////////////
	// VertexBuffer /////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size)
	{
		VkDevice* device = VulkanContext::GetContext()->GetDevice();

		m_BufferMemorySize = size;

		VulkanUtility::CreateBuffer(m_BufferMemorySize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_BufferMemory);

	}

	VulkanVertexBuffer::VulkanVertexBuffer(float * vertices, uint32_t size)
	{
		m_Vertices.resize(size / sizeof(float));
		for (int i = 0; i < m_Vertices.size(); i++)
		{
			m_Vertices[i] = *(vertices + i);
		}
		VkDevice* device = VulkanContext::GetContext()->GetDevice();
		size_t actualSize = sizeof(m_Vertices[0]) * m_Vertices.size();
		m_BufferMemorySize = actualSize;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		VulkanUtility::CreateBuffer(m_BufferMemorySize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(*device, stagingBufferMemory, 0, actualSize, 0, &data);
		memcpy(data, m_Vertices.data(), actualSize);
		vkUnmapMemory(*device, stagingBufferMemory);

		VulkanUtility::CreateBuffer(m_BufferMemorySize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_BufferMemory);

		VulkanUtility::CopyBuffer(stagingBuffer, m_Buffer, actualSize);

		vkDestroyBuffer(*device, stagingBuffer, nullptr);
		vkFreeMemory(*device, stagingBufferMemory, nullptr);

	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		VkDevice* device = VulkanContext::GetContext()->GetDevice();
		vkDestroyBuffer(*device, m_Buffer, nullptr);
		vkFreeMemory(*device, m_BufferMemory, nullptr);
	}

	void VulkanVertexBuffer::Bind() const
	{
	}

	void VulkanVertexBuffer::Unbind() const
	{
	}

	void VulkanVertexBuffer::SetData(const void* data, uint32_t size)
	{
		VkDevice* device = VulkanContext::GetContext()->GetDevice();
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		VulkanUtility::CreateBuffer(m_BufferMemorySize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* stagingData;
		vkMapMemory(*device, stagingBufferMemory, 0, size, 0, &stagingData);
		memcpy(stagingData, data, size);
		vkUnmapMemory(*device, stagingBufferMemory);
		VulkanUtility::CopyBuffer(stagingBuffer, m_Buffer, size);

		vkDestroyBuffer(*device, stagingBuffer, nullptr);
		vkFreeMemory(*device, stagingBufferMemory, nullptr);
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
		VkDevice* device = VulkanContext::GetContext()->GetDevice();
		size_t actualSize = sizeof(m_Indices[0]) * m_Indices.size();
		m_BufferMemorySize = actualSize;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		VulkanUtility::CreateBuffer(m_BufferMemorySize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(*device, stagingBufferMemory, 0, actualSize, 0, &data);
		memcpy(data, m_Indices.data(), actualSize);
		vkUnmapMemory(*device, stagingBufferMemory);

		VulkanUtility::CreateBuffer(m_BufferMemorySize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_BufferMemory);

		VulkanUtility::CopyBuffer(stagingBuffer, m_Buffer, actualSize);

		vkDestroyBuffer(*device, stagingBuffer, nullptr);
		vkFreeMemory(*device, stagingBufferMemory, nullptr);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		VkDevice* device = VulkanContext::GetContext()->GetDevice();
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
		:m_Name(name), m_BufferMemorySize(size)
	{
		uint32_t swapImageCount = VulkanContext::GetContext()->GetSwapChain()->GetImageCount();
		VkDevice* device = VulkanContext::GetContext()->GetDevice();

		m_Buffers.resize(swapImageCount);
		VkDeviceSize requiredSize = size;
		VulkanUtility::CreateBuffer(requiredSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Buffers, m_BufferMemory);

		if (m_BufferMemorySize < requiredSize) m_BufferMemorySize = static_cast<uint32_t>(requiredSize);

		vkMapMemory(*device, m_BufferMemory, 0, m_BufferMemorySize, 0, &m_MappedMemory);

	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		VkDevice* device = VulkanContext::GetContext()->GetDevice();
		vkUnmapMemory(*device, m_BufferMemory);

		for (auto buffer : m_Buffers)
		{
			vkDestroyBuffer(*device, buffer, nullptr);
		}
		vkFreeMemory(*device, m_BufferMemory, nullptr);
	}

	void VulkanUniformBuffer::Bind()
	{
	}

	void VulkanUniformBuffer::Unbind()
	{
	}

	void VulkanUniformBuffer::SetLayout(const BufferLayout & layout)
	{
		m_Layout = layout;
	}

	void VulkanUniformBuffer::UpdateMat4(std::string name, glm::mat4 matrix)
	{
		HZ_PROFILE_FUNCTION();
		for (auto element : m_Layout.GetElements())
		{
			if (element.Name == name)
			{
				VkDeviceSize offset = m_BufferMemorySize * VulkanRendererAPI::GetFrame()->imageIndex + element.Offset;
				memcpy((char*)m_MappedMemory + offset, &matrix, element.Size);
				break;
			}
		}
	}

	void VulkanUniformBuffer::UpdateFloat4(std::string name, glm::vec4 vector)
	{
		HZ_PROFILE_FUNCTION();
		for (auto element : m_Layout.GetElements())
		{
			if (element.Name == name)
			{
				VkDeviceSize offset = m_BufferMemorySize * VulkanRendererAPI::GetFrame()->imageIndex + element.Offset;
				memcpy((char*)m_MappedMemory + offset, &vector, element.Size);
				break;
			}
		}
	}

	std::string VulkanUniformBuffer::GetName()
	{
		return m_Name;
	}

}