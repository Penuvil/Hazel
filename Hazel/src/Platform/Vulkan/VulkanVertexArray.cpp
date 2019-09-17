#include "hzpch.h"

#include "VulkanVertexArray.h"

namespace Hazel {

	VulkanVertexArray::VulkanVertexArray()
	{
	}

	VulkanVertexArray::~VulkanVertexArray()
	{
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

		//Broken -- needs to be fixed to make format correct

		m_VertexInputBindingDescription.binding = 0;
		m_VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		m_VertexInputBindingDescription.stride = layout.GetStride();

		m_VertexInputAttributeDescriptions[0].binding = 0;
		m_VertexInputAttributeDescriptions[0].location = 0;
		m_VertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		m_VertexInputAttributeDescriptions[0].offset = layout.GetElements()[0].Offset;

		m_VertexInputAttributeDescriptions[1].binding = 0;
		m_VertexInputAttributeDescriptions[1].location = 1;
		m_VertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		m_VertexInputAttributeDescriptions[1].offset = layout.GetElements()[1].Offset;

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void VulkanVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		m_IndexBuffer = indexBuffer;
	}

}