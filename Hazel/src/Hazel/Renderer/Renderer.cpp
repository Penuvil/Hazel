#include "hzpch.h"

#include "Hazel/Renderer/Renderer.h"
#include "Hazel/Renderer/RenderCommand.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Hazel/Renderer/Renderer2D.h"

namespace Hazel {

	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();

	void Renderer::Init()
	{
		HZ_PROFILE_FUNCTION();

		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
		RenderCommand::Shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(Ref<OrthographicCamera> camera)
	{
		s_SceneData->ViewProjectionMatrix = camera->GetViewProjectionMatrix();
		RenderCommand::BeginScene();
	}

	void Renderer::EndScene()
	{
		RenderCommand::EndScene();
	}

	void Renderer::SetClearColor(const glm::vec4 & color)
	{
		RenderCommand::SetClearColor(color);
	}

	void Renderer::Clear()
	{
		RenderCommand::Clear();
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceId, const glm::vec4& fragColor, const glm::mat4& transform)
	{
		RenderCommand::Submit(shader, vertexArray, instanceId, fragColor, transform, s_SceneData->ViewProjectionMatrix);
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, Ref<Texture2D> texture, uint32_t instanceId, const glm::vec4 & fragColor, const glm::mat4 & transform)
	{
		RenderCommand::Submit(shader, vertexArray, texture, instanceId, fragColor, transform, s_SceneData->ViewProjectionMatrix);
	}

}