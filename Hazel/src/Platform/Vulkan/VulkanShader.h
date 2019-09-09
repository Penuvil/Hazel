#pragma once

#include "Hazel/Renderer/Shader.h"

namespace Hazel {

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& filepath);
		VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~VulkanShader();

		// Inherited via Shader
		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const std::string& GetName() const override { return m_Name; }

		virtual void SetUniformBuffers(const Ref<std::unordered_map<std::string, Ref<UniformBuffer>>> buffers) override;
		virtual	Ref<UniformBuffer> GetUniformBuffer(const std::string& name) const override;

	private:
		std::string m_Name;
		Ref<std::unordered_map<std::string, Ref<UniformBuffer>>> m_UniformBuffers;
	};
}