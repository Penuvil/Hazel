#include "hzpch.h"

#include "Platform/Vulkan/VulkanShader.h"

#include <shaderc/shaderc.hpp>

namespace Hazel {
	VulkanShader::VulkanShader(const std::string& vertexSrc, const std::string& fragmentSource)
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

}