#pragma once

#include "Hazel/Renderer/VertexArray.h"

#include "vulkan/vulkan.h"

namespace Hazel {
	class VulkanVertexArray : public VertexArray
	{
	public:
		VulkanVertexArray();
		virtual ~VulkanVertexArray();

		void CreateDescriptorSets();
		inline const std::vector<VkDescriptorSet>* GetDescriptorSets() { return &m_DecsriptorSets; }

		// Inherited via VertexArray
		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }
	private:
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::vector<VkDescriptorSet> m_DecsriptorSets;

	};
}