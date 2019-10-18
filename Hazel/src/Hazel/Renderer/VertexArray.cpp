#include "hzpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Platform/Vulkan/VulkanVertexArray.h"

namespace Hazel {

	Ref<VertexArray> VertexArray::Create(uint32_t numberOfInstances)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLVertexArray>();
			case RendererAPI::API::Vulkan:	return std::make_shared<VulkanVertexArray>(numberOfInstances);
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}