#pragma once

#include "Hazel/Renderer/VertexArray.h"

#include "vulkan/vulkan.h"

namespace Hazel {
	class VulkanVertexArray : public VertexArray
	{
	public:
		VulkanVertexArray(uint32_t& numberOfInstances);
		virtual ~VulkanVertexArray();

		void CreateDescriptorSets(uint32_t instanceIndex);
		inline const std::vector<VkDescriptorSet>* GetDescriptorSets(uint32_t instance) { return &m_DecsriptorSets[instance]; }

		// Inherited via VertexArray
		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }
		virtual Ref<UniformBuffer> GetUniformBuffer(uint32_t instance, std::string name) override { return m_UniformBuffers[instance].find(name)->second; }
		VkDescriptorSet GetDescriptorSet(uint32_t instance, uint32_t swapIndex) { return m_DecsriptorSets[instance][swapIndex]; }
	private:
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::vector<std::unordered_map<std::string, Ref<UniformBuffer>>> m_UniformBuffers;
		std::vector<std::vector<VkDescriptorSet>> m_DecsriptorSets;

	};
}