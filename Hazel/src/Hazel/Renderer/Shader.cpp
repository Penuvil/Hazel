#include "hzpch.h"
#include "Shader.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vulkan/VulkanShader.h"

namespace Hazel {
	Shader::~Shader()
	{
	}

	Ref<Shader> Shader::Create(const std::string& filepath, const BufferLayout& vertexBufferLayout)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLShader>(filepath);
			case RendererAPI::API::Vulkan:	return std::make_shared<VulkanShader>(filepath, vertexBufferLayout);
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc, const BufferLayout& vertexBufferLayout)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
			case RendererAPI::API::Vulkan:  return std::make_shared<VulkanShader>(name, vertexSrc, fragmentSrc, vertexBufferLayout);
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	ShaderLibrary* ShaderLibrary::s_ShaderLibrary = nullptr;

	ShaderLibrary::ShaderLibrary()
	{
		s_ShaderLibrary = this;
		Ref<UniformBuffer> matricesUniformBuffer;
		matricesUniformBuffer.reset(UniformBuffer::Create("Matrices", 2 * ShaderDataTypeSize(ShaderDataType::Mat4), 0));

		Ref<UniformBuffer> colorUniformBuffer;
		colorUniformBuffer.reset(UniformBuffer::Create("Color", ShaderDataTypeSize(ShaderDataType::Float3), 0));

		m_UniformBuffers.reset(new std::unordered_map<std::string, Ref<UniformBuffer>>());
		AddUniformBuffer(matricesUniformBuffer);
		AddUniformBuffer(colorUniformBuffer);
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		HZ_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
		shader->SetUniformBuffers(m_UniformBuffers);
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	void ShaderLibrary::AddUniformBuffer(const Ref<UniformBuffer>& buffer)
	{		
		HZ_CORE_ASSERT(m_UniformBuffers->find(buffer->GetName()) == m_UniformBuffers->end(), "Uniform buffer already exists! {0}", buffer->GetName());
		m_UniformBuffers->insert({ buffer->GetName(), buffer });		
	}

	Hazel::Ref<Hazel::Shader> ShaderLibrary::Load(const std::string& filepath, const BufferLayout& vertexBufferLayout)
	{
		auto shader = Shader::Create(filepath, vertexBufferLayout);
		Add(shader);
		return shader;
	}

	Hazel::Ref<Hazel::Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath, const BufferLayout& vertexBufferLayout)
	{
		auto shader = Shader::Create(filepath, vertexBufferLayout);
		Add(name, shader);
		return shader;
	}

	Hazel::Ref<Hazel::Shader> ShaderLibrary::Get(const std::string& name)
	{
		HZ_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	Ref<UniformBuffer> ShaderLibrary::GetUniformBuffer(const std::string & name)
	{
		return m_UniformBuffers->find(name)->second;
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}

}