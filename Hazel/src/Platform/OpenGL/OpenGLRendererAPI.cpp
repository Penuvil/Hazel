#include "hzpch.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

#include "Platform/OpenGL/OpenGLTexture.h"
//#include "Hazel/Renderer/Buffer.h"


#include <glad/glad.h>

namespace Hazel {

	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::BeginScene()
	{
	}

	void OpenGLRendererAPI::EndScene()
	{
	}

		void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceId, const glm::vec4& fragColor, const glm::mat4 & transform, const glm::mat4 & viewProjection)
	{
		shader->Bind();
		vertexArray->GetUniformBuffer(0, "Matrices")->Bind();

		glBufferSubData(GL_UNIFORM_BUFFER, 0, ShaderDataTypeSize(ShaderDataType::Mat4), &viewProjection);
		glBufferSubData(GL_UNIFORM_BUFFER, ShaderDataTypeSize(ShaderDataType::Mat4), ShaderDataTypeSize(ShaderDataType::Mat4), &transform);

		vertexArray->GetUniformBuffer(0, "Color")->Bind();

		glBufferSubData(GL_UNIFORM_BUFFER, 0, ShaderDataTypeSize(ShaderDataType::Float4), &fragColor);

		vertexArray->Bind(instanceId);
		DrawIndexed(vertexArray, 0);
	}

	void OpenGLRendererAPI::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, Ref<Texture2D> texture, uint32_t instanceId, const glm::vec4 & fragColor, const glm::mat4 & transform, const glm::mat4 & viewProjection)
	{

		std::static_pointer_cast<OpenGLTexture2D>(texture)->Bind(0);
		Submit(shader, vertexArray, instanceId, fragColor, transform, viewProjection);
	}

	void OpenGLRendererAPI::BeginRender()
	{
	}

	void OpenGLRendererAPI::EndRender()
	{
	}

	void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceId)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}