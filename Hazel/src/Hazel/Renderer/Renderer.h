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

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

//		static void AddUniformBuffer(Hazel::Ref<UniformBuffer> buffer);
		static void SetClearColor(const glm::vec4 & color);
		static void Clear();
		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* s_SceneData;
	};


}
