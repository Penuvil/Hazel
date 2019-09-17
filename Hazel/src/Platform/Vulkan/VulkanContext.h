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

		inline VkDevice* GetDevice() { return &m_LogicalDevice; }
		inline VkPhysicalDevice* GetPhysicalDevice() { return &m_PhysicalDevice; }
		inline VkCommandPool* GetCommandPool() { return &m_CommandPool; }
		inline VkQueue* GetGraphicsQueue() { return &m_GraphicsQueue; }
		inline Ref<VulkanSwapChain> GetSwapChain() { return m_SwapChain; }

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageSeverityFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
			void* userData);
		inline static VulkanContext& GetContext() { return *s_Context; }
	private:
		const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		GLFWwindow* m_WindowHandle;
		VkInstance m_Instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_LogicalDevice = VK_NULL_HANDLE;
		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
		VkQueue m_PresentQueue = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkCommandPool m_CommandPool = VK_NULL_HANDLE;
		Ref<VulkanSwapChain> m_SwapChain;

		static VulkanContext* s_Context;



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
		void CreateCommandPool();

	};

}