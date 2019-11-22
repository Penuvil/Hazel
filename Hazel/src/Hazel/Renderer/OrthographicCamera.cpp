#include "hzpch.h"

#include "Hazel/Renderer/OrthographicCamera.h"
#include "Hazel/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLCamera.h"
#include "Platform/Vulkan/VulkanCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {

	Ref<OrthographicCamera> OrthographicCamera::Create(float left, float right, float bottom, float top)
	{		
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLOrthographicCamera>(left, right, bottom, top);
		case RendererAPI::API::Vulkan:	return std::make_shared<VulkanOrthographicCamera>(left, right, bottom, top);
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}