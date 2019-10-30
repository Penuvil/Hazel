#pragma once

#include "Hazel/Renderer/RendererAPI.h"
#include "Hazel/Renderer/Texture.h"

#include <vulkan/vulkan.h>

namespace Hazel {
	class VulkanRendererAPI : public RendererAPI
	{
	public:

		virtual ~VulkanRendererAPI();

		struct FrameInfo {
			uint32_t imageIndex;
			size_t frameIndex;
			VkClearValue* clearColor;
			VkSemaphore* imageAvailableSemaphore;
			VkSemaphore* layerCompleteSemaphore;
			VkSemaphore* renderFinishedSemaphore;
			VkFence* inFlightFence;
		};

		struct BatchInfo {
			VkCommandBuffer commandBuffer;
			Ref<Shader> shader;
		};

		static inline const Ref<FrameInfo> GetFrame() { return s_CurrentFrame; }
		static inline const Ref<BatchInfo> GetBatch() { return s_CurrentBatch; }
		static inline void SetBatchShader(const std::shared_ptr<Shader>& shader) { s_CurrentBatch->shader = shader; }

		// Inherited via RendererAPI
		virtual void Init() override;
		virtual void BeginScene() override;
		virtual void EndScene() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void SetClearColor(const glm::vec4 & color) override;
		virtual void Clear() override;
		virtual void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceId, const glm::vec4& fragColor, const glm::mat4& transform, const glm::mat4& viewProjection) override;
		virtual void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray,Ref<Texture2D> texture, uint32_t instanceId, const glm::vec4& fragColor, const glm::mat4& transform, const glm::mat4& viewProjection) override;
		virtual void BeginRender() override;
		virtual void EndRender() override;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceId) override;

		void Shutdown();
	private:
		const int MAX_FRAMES_IN_FLIGHT = 5;
		size_t m_FrameIndex = 0;
		glm::vec4 m_ClearColor;
		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_LayerCompleteSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;
		VkViewport m_Viewport;
		bool m_ResizeEvent;

	private:
		static Ref<FrameInfo> s_CurrentFrame;
		static Ref<BatchInfo> s_CurrentBatch;
	};
}