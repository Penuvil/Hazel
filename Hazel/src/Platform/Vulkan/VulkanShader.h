#pragma once

#include "Hazel/Renderer/Shader.h"

namespace Hazel {

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& vertsxSrc, const std::string& fragmentSource);
		virtual ~VulkanShader();

		// Inherited via Shader
		virtual void Bind() const override;

		virtual void Unbind() const override;

	};
}