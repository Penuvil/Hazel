#include "hzpch.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

#include <glad/glad.h>

namespace Hazel {

	/////////////////////////////////////////////////////////////////////////////
	// VertexBuffer /////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		HZ_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		HZ_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		HZ_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		HZ_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		HZ_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	/////////////////////////////////////////////////////////////////////////////
	// IndexBuffer //////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		HZ_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		
		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		HZ_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		HZ_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		HZ_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	/////////////////////////////////////////////////////////////////////////////
	// UniformBuffer ////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	OpenGLUniformBuffer::OpenGLUniformBuffer(std::string name, uint32_t size, uint32_t shaderBlock)
		: m_Name(name), m_ShaderBlock(shaderBlock)
	{
		glCreateBuffers(1, &m_Buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer);
		glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_Buffer);
	}

	void OpenGLUniformBuffer::Bind()
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, m_ShaderBlock, m_Buffer);
	}

	void OpenGLUniformBuffer::Unbind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLUniformBuffer::UpdateMat4(std::string name, glm::mat4 matrix)
	{
		for (auto element : m_Layout.GetElements())
		{
			if (element.Name == name)
			{
				glBufferSubData(GL_UNIFORM_BUFFER, element.Offset, element.Size, &matrix);
				break;
			}
		}
	}

	void OpenGLUniformBuffer::UpdateFloat4(std::string name, glm::vec4 vector)
	{
		for (auto element : m_Layout.GetElements())
		{
			if (element.Name == name)
			{
				glBufferSubData(GL_UNIFORM_BUFFER, element.Offset, element.Size, &vector);
				break;
			}
		}
	}



}
