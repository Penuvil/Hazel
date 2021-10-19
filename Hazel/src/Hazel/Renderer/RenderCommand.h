#pragma once

#include "Hazel/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Hazel/Renderer/RendererAPI.h"


namespace Hazel {

	class RenderCommand
	{
	public:
		static void Init()
		{
			switch (RendererAPI::GetAPI())
			{
			case RendererAPI::API::OpenGL:
				s_RendererAPI = std::make_unique<OpenGLRendererAPI>();
				break;
			case RendererAPI::API::Vulkan:
				s_RendererAPI = std::make_unique<VulkanRendererAPI>();
				break;
			}
			s_RendererAPI->Init();
		}

		static void BeginScene()
		{
			s_RendererAPI->BeginScene();
		}

		static void EndScene()
		{
			s_RendererAPI->EndScene();
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		static void Clear()
		{
			s_RendererAPI->Clear();
		}

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceId, const glm::vec4& fragColor, const glm::mat4& transform, const glm::mat4& viewProjection)
		{
			s_RendererAPI->Submit(shader, vertexArray, instanceId, fragColor, transform, viewProjection);
		}

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, Ref<Texture2D> texture, uint32_t instanceId, const glm::vec4& fragColor, const glm::mat4& transform, const glm::mat4& viewProjection)
		{
			s_RendererAPI->Submit(shader, vertexArray, texture, instanceId, fragColor, transform, viewProjection);
		}

//		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceId)
//		{
//			s_RendererAPI->DrawIndexed(vertexArray, instanceId);
//		}

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, count);
			
		}

		inline static void Shutdown()
		{
			s_RendererAPI.reset();
		}
		
	private:
		static Scope<RendererAPI> s_RendererAPI;
	};

}
