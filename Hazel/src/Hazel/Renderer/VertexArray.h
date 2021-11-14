#pragma once

#include <memory>
#include "Hazel/Renderer/Buffer.h"

namespace Hazel {

	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void Bind(uint32_t instanceId) = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;
		virtual Ref<UniformBuffer> GetUniformBuffer(uint32_t instance, std::string name) = 0;

		static Ref<VertexArray> Create(uint32_t numberOfInstances);
	};

}
