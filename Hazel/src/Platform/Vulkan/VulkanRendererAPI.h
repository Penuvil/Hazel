#pragma once

#include "Hazel/Renderer/RendererAPI.h"

#include <vulkan/vulkan.h>

namespace Hazel {
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		struct FrameInfo {
			uint32_t imageIndex;
			size_t frameIndex;
			VkClearValue* clearColor;
			VkSemaphore* imageAvailableSemaphore;
			VkSemaphore* layerCompleteSemaphore;
			VkSemaphore* renderFinishedSemaphore;
			VkFence* inFlightFence;
		};

		static inline const Ref<FrameInfo> GetFrame() { return s_CurrentFrame; }

		// Inherited via RendererAPI
		virtual void Init() override;
		virtual void BeginScene() override;
		virtual void EndScene() override;
		virtual void SetClearColor(const glm::vec4 & color) override;
		virtual void Clear() override;
		virtual void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceId, const glm::vec3& fragColor, const glm::mat4& transform, const glm::mat4& viewProjection) override;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
	private:
		const int MAX_FRAMES_IN_FLIGHT = 3;
		size_t m_FrameIndex = 0;
		glm::vec4 m_ClearColor;
		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_LayerCompleteSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;

	private:
		static Ref<FrameInfo> s_CurrentFrame;
	};
}