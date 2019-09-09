#include "hzpch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Hazel {

	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	//void Renderer::AddUniformBuffer(Hazel::Ref<UniformBuffer> buffer)
	//{
	//	RenderCommand::AddUniformBuffer(buffer);
	//}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		RenderCommand::Submit(shader, vertexArray, transform, s_SceneData->ViewProjectionMatrix);
//		shader->Bind();
// 		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
//		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);
//
//		vertexArray->Bind();
//		RenderCommand::DrawIndexed(vertexArray);
	}

}