#pragma once

//#include "RenderCommand.h"

#include "OrthographicCamera.h"
#include "Shader.h"
#include "VertexArray.h"
#include "RendererAPI.h"

namespace Hazel {

	class Renderer
	{
	public:
		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

//		static void AddUniformBuffer(Hazel::Ref<UniformBuffer> buffer);
		static void SetClearColor(const glm::vec4 & color);
		static void Clear();
		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceId, const glm::vec4& fragColor, const glm::mat4& transform = glm::mat4(1.0f));
		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, Ref<Texture2D> texture, uint32_t instanceId, const glm::vec4& fragColor, const glm::mat4& transform = glm::mat4(1.0f));
		static void BeginRender();
		static void EndRender();


		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static Scope<SceneData> s_SceneData;
	};
}
