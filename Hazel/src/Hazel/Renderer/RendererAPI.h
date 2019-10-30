#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

namespace Hazel {
	
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1, Vulkan = 2
		};
	public:
		virtual ~RendererAPI() = default;
		virtual void Init() = 0;

		virtual void BeginScene() = 0;
		virtual void EndScene() = 0;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
//		virtual void AddUniformBuffer(Hazel::Ref<UniformBuffer> buffer) = 0;
		virtual void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceId, const glm::vec4& fragColor, const glm::mat4& transform, const glm::mat4& viewProjection) = 0;
		virtual void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, Ref<Texture2D> texture, uint32_t instanceId, const glm::vec4& fragColor, const glm::mat4& transform, const glm::mat4& viewProjection) = 0;
		virtual void BeginRender() = 0;
		virtual void EndRender() = 0;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceId) = 0;

		inline static API GetAPI() { return s_API; }
		static void SetAPI(API api);
	private:
		static API s_API;
	};

}