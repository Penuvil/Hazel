#pragma once

#include "Hazel/Renderer/Shader.h"
#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>


namespace Hazel {

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& filepath, const BufferLayout& vertexBufferLayout);
		VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc, const BufferLayout& vertexBufferLayout);
		virtual ~VulkanShader();

		inline  VkPipeline* GetGraphicsPipeline() { return &m_GraphicsPipeline; }
		inline VkPipelineLayout* GetGraphicsPipelineLayout() { return &m_PipelineLayout; }
//		inline std::vector<VkDescriptorSet>* GetDescriptorSets() { return &m_DecsriptorSets; }

		// Inherited via Shader
		virtual void Bind() override;
		virtual void Unbind() const override;

		virtual void SetInt(const std::string& name, int value) override {}
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override {}
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override {}
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override {}

		virtual const std::string& GetName() const override { return m_Name; }

//		virtual void SetUniformBuffers(const Ref<std::unordered_map<std::string, Ref<UniformBuffer>>> buffers) override;
//		virtual	Ref<UniformBuffer> GetUniformBuffer(const std::string& name) const override;
	private:
		VkShaderModule CreateShaderModule(std::vector<uint32_t>& code);
		std::vector<uint32_t> Compile(const std::string& shaderSource, shaderc_shader_kind shaderType);
		void CreateGraphicsPipeline(const std::string& vertexSrc, const std::string& fragmentSrc, const BufferLayout& vertexBufferLayout);
	private:
		std::string m_Name;
//		Ref<std::unordered_map<std::string, Ref<UniformBuffer>>> m_UniformBuffers;
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;
	};
}