#pragma once

#include "Hazel/Renderer/RendererAPI.h"

namespace Hazel {
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		// Inherited via RendererAPI
		virtual void Init() override;

		virtual void SetClearColor(const glm::vec4 & color) override;

		virtual void Clear() override;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;

	};
}