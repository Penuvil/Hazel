#pragma once

#include "Hazel/Renderer/RendererAPI.h"
#include "Hazel/Renderer/OrthographicCamera.h"

namespace Hazel {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void BeginScene() override;
		virtual void EndScene() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
//		virtual void AddUniformBuffer(Hazel::Ref<UniformBuffer> buffer) override;
		virtual void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceId, const glm::vec4& fragColor, const glm::mat4& transform, const glm::mat4& viewProjection) override;
		virtual void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, Ref<Texture2D> texture, uint32_t instanceId, const glm::vec4& fragColor, const glm::mat4& transform, const glm::mat4& viewProjection) override;
		virtual void BeginRender() override;
		virtual void EndRender() override;


		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceId) override;
	private:
//		std::unordered_map<std::string, Hazel::Ref<UniformBuffer>> m_UniformBuffers;
	};


}
