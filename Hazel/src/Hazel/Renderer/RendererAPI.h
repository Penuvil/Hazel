#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"
#include "Shader.h"

namespace Hazel {
	
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1, Vulkan = 2
		};
	public:
		virtual void Init() = 0;
		virtual void BeginScene() = 0;
		virtual void EndScene() = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
//		virtual void AddUniformBuffer(Hazel::Ref<UniformBuffer> buffer) = 0;
		virtual void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform, const glm::mat4& viewProjection) = 0;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;

		inline static API GetAPI() { return s_API; }
		static void SetAPI(API api);
	private:
		static API s_API;
	};

}