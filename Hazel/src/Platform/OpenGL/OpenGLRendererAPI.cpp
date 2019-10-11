#include "hzpch.h"
#include "OpenGLRendererAPI.h"
//#include "Hazel/Renderer/Buffer.h"


#include <glad/glad.h>

namespace Hazel {

	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenGLRendererAPI::BeginScene()
	{
	}

	void OpenGLRendererAPI::EndScene()
	{
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceId, const glm::vec3& fragColor, const glm::mat4 & transform, const glm::mat4 & viewProjection)
	{
		shader->Bind();
		vertexArray->GetUniformBuffer(0, "Matrices")->Bind();

		glBufferSubData(GL_UNIFORM_BUFFER, 0, ShaderDataTypeSize(ShaderDataType::Mat4), &viewProjection);
		glBufferSubData(GL_UNIFORM_BUFFER, ShaderDataTypeSize(ShaderDataType::Mat4), ShaderDataTypeSize(ShaderDataType::Mat4), &transform);

		vertexArray->GetUniformBuffer(0, "Color")->Bind();

		glBufferSubData(GL_UNIFORM_BUFFER, 0, ShaderDataTypeSize(ShaderDataType::Float3), &fragColor);

		vertexArray->Bind();
		DrawIndexed(vertexArray);
	}

	void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

}