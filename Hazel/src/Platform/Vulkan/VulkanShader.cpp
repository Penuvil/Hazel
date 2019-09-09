#include "hzpch.h"

#include "Platform/Vulkan/VulkanShader.h"

#include <shaderc/shaderc.hpp>

namespace Hazel {

	
	VulkanShader::VulkanShader(const std::string & filepath)
	{
	
	}

	VulkanShader::VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSource)
	{

		shaderc::Compiler shaderCompiler;
		shaderc::CompileOptions compileOptions;
		shaderc::SpvCompilationResult spvCompilationResult;
		spvCompilationResult = shaderCompiler.CompileGlslToSpv(vertexSrc, shaderc_vertex_shader, "vertexShader", "main", compileOptions);
	}

	VulkanShader::~VulkanShader()
	{
	}

	void VulkanShader::Bind() const
	{
	}

	void VulkanShader::Unbind() const
	{
	}

	void VulkanShader::SetUniformBuffers(const Ref<std::unordered_map<std::string, Ref<UniformBuffer>>> buffers)
	{
		m_UniformBuffers = buffers;
	}

	Ref<UniformBuffer> VulkanShader::GetUniformBuffer(const std::string & name) const
	{
		HZ_CORE_ASSERT(m_UniformBuffers->find(name) != m_UniformBuffers->end(), "Uniform buffer not found! {0}", name);
		return m_UniformBuffers->find(name)->second;
	}

}