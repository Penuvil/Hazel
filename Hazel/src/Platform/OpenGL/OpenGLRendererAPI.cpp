#include "hzpch.h"
#include "OpenGLRendererAPI.h"
//#include "Hazel/Renderer/Buffer.h"

#include <glad/glad.h>

namespace Hazel {

	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Ref<UniformBuffer> uniformBuffer;
		uniformBuffer.reset(UniformBuffer::Create("Matrices", 2 * ShaderDataTypeSize(ShaderDataType::Mat4), 0));
		AddUniformBuffer(uniformBuffer);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::AddUniformBuffer(Hazel::Ref<UniformBuffer> buffer)
	{
		HZ_CORE_ASSERT(m_UniformBuffers.find(buffer->GetName()) == m_UniformBuffers.end(), "Uniform buffer already exists! {0}", buffer->GetName());
		m_UniformBuffers[buffer->GetName()] = buffer;
	}

	void OpenGLRendererAPI::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4 & transform, const glm::mat4 & viewProjection)
	{
		shader->Bind();
		m_UniformBuffers["Matrices"]->Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, 0, ShaderDataTypeSize(ShaderDataType::Mat4), &viewProjection);
		glBufferSubData(GL_UNIFORM_BUFFER, ShaderDataTypeSize(ShaderDataType::Mat4), ShaderDataTypeSize(ShaderDataType::Mat4), &transform);

		vertexArray->Bind();
		DrawIndexed(vertexArray);
	}

	void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

}