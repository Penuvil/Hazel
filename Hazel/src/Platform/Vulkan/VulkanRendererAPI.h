#pragma once

#include "Hazel/Renderer/RendererAPI.h"

#include <vulkan/vulkan.h>

namespace Hazel {
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		// Inherited via RendererAPI
		virtual void Init() override;

		virtual void SetClearColor(const glm::vec4 & color) override;
		virtual void Clear() override;
//		virtual void AddUniformBuffer(Hazel::Ref<UniformBuffer> buffer) override;
		virtual void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform, const glm::mat4& viewProjection) override;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
	private:
		const int MAX_FRAMES_IN_FLIGHT = 2;
		size_t m_CurrentFrame = 0;
		glm::vec4 m_ClearColor;
		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;
	};
}