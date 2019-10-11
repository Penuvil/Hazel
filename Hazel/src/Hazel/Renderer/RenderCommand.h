#pragma once

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Hazel {

	class RenderCommand
	{
	public:
		inline static void Init()
		{
			switch (RendererAPI::GetAPI())
			{
			case RendererAPI::API::OpenGL:
				s_RendererAPI = new OpenGLRendererAPI();
				break;
			case RendererAPI::API::Vulkan:
				s_RendererAPI = new VulkanRendererAPI();
				break;
			}
			s_RendererAPI->Init();
		}

		inline static void BeginScene()
		{
			s_RendererAPI->BeginScene();
		}

		inline static void EndScene()
		{
			s_RendererAPI->EndScene();
		}

		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceId, const glm::vec3& fragColor, const glm::mat4& transform, const glm::mat4& viewProjection)
		{
			s_RendererAPI->Submit(shader, vertexArray, instanceId, fragColor, transform, viewProjection);
		}

		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};

}
