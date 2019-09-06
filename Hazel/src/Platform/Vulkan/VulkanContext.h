#pragma once

#include "Hazel/Renderer/GraphicsContext.h"
#include "Platform/Vulkan/VulkanSwapChain.h"

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Hazel {

	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext(GLFWwindow* windowHandle);
		virtual ~VulkanContext();

		virtual void Init() override;
		virtual void SwapBuffers() override;

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageSeverityFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
			void* userData);
	private:
		const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		GLFWwindow* m_WindowHandle;
		VkInstance m_Instance = nullptr;
		VkDebugUtilsMessengerEXT m_debugMessenger = nullptr;
		VkPhysicalDevice m_PhysicalDevice = nullptr;
		VkDevice m_LogicalDevice = nullptr;
		VkQueue m_GraphicsQueue = nullptr;
		VkQueue m_PresentQueue = nullptr;
		VkSurfaceKHR m_Surface = nullptr;
		Ref<VulkanSwapChain> m_SwapChain;

#ifdef HZ_DEBUG
		const bool m_EnableValidationLayers = true;
#else
		const bool m_EnableValidationLayers = false;
#endif

		void CreateInstance(uint32_t* vulkanVersion);
		bool CheckValidationLayerSupport();
		std::vector<const char*> GetRequiredExtensions();
		void SetupDebugMessenger();
		uint32_t EvaluateDevice(VkPhysicalDevice device);
		void SelectPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSurface();

	};

}