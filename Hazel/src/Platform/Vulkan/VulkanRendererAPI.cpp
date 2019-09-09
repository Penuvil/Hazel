#include "hzpch.h"
#include "VulkanRendererAPI.h"

namespace Hazel {

	void VulkanRendererAPI::Init()
	{
	}

	void VulkanRendererAPI::SetClearColor(const glm::vec4 & color)
	{
	}

	void VulkanRendererAPI::Clear()
	{
	}

	void VulkanRendererAPI::AddUniformBuffer(Hazel::Ref<UniformBuffer> buffer)
	{
	}

	void VulkanRendererAPI::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4 & transform, const glm::mat4 & viewProjection)
	{
	}

	void VulkanRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
	}

}