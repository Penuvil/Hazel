#include "hzpch.h"
#include "VulkanContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <vulkan/vulkan.h>

namespace Hazel {

	VulkanContext::VulkanContext(GLFWwindow* windowHandle)
		:m_WindowHandle(windowHandle)
	{
		HZ_CORE_ASSERT(windowHandle, "Window handle is null")
	}

	void VulkanContext::Init()
	{
		
	}
}