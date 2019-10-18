#include "hzpch.h"

#include <fstream>

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

	std::string Shader::ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
			;
		}
		else
		{
			HZ_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}


	ShaderLibrary* ShaderLibrary::s_ShaderLibrary = nullptr;

	ShaderLibrary::ShaderLibrary()
	{
		s_ShaderLibrary = this;
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		HZ_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
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

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		HZ_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}

}