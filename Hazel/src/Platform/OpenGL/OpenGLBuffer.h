#pragma once

#include "Hazel/Renderer/Buffer.h"

namespace Hazel {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const;
		virtual void Unbind() const;

		virtual uint32_t GetCount() const { return m_Count; }
	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};

	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(std::string name, uint32_t size, uint32_t shaderBlock);
		virtual ~OpenGLUniformBuffer();

		// Inherited via UniformBuffer
		virtual void Bind() override;
		virtual void Unbind() override;

//		inline virtual std::variant<unsigned int, VkBuffer> GetBuffer() override { return m_Buffer; }
		inline virtual std::string GetName() override { return m_Name; }
	private:
		std::string m_Name;
		unsigned int m_Buffer;
		uint32_t m_ShaderBlock;

	};
}