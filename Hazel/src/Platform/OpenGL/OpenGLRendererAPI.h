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
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
//		virtual void AddUniformBuffer(Hazel::Ref<UniformBuffer> buffer) override;
		virtual void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceId, const glm::vec3& fragColor, const glm::mat4& transform, const glm::mat4& viewProjection) override;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
	private:
//		std::unordered_map<std::string, Hazel::Ref<UniformBuffer>> m_UniformBuffers;
	};


}
