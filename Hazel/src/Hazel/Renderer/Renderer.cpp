#include "hzpch.h"
#include "Renderer.h"
#include "RenderCommand.h"
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

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceId, const glm::vec3& fragColor, const glm::mat4& transform)
	{
		RenderCommand::Submit(shader, vertexArray, instanceId, fragColor, transform, s_SceneData->ViewProjectionMatrix);
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, Ref<Texture2D> texture, uint32_t instanceId, const glm::vec3 & fragColor, const glm::mat4 & transform)
	{
		RenderCommand::Submit(shader, vertexArray, texture, instanceId, fragColor, transform, s_SceneData->ViewProjectionMatrix);
	}

	void Renderer::BeginRender()
	{
		RenderCommand::BeginRender();
	}

	void Renderer::EndRender()
	{
		RenderCommand::EndRender();
	}
}