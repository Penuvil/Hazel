#pragma once
#include "Platform/Vulkan/VulkanUtility.h"

namespace Hazel
{
	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(GLFWwindow* windowHandle, VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface);
		void Destroy();

		inline uint32_t GetImageCount() { return static_cast<uint32_t>(m_SwapChainImages.size()); }
		inline const VkExtent2D* GetExtent2D() { return &m_SwapChainExtent; } 
	private:
		void Init();
		void CreateSwapChain();
		void CreateImageViews();
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& supportedFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& supportedPresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		
	private:
		GLFWwindow* m_WindowHandle;
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_Device;
		VkSurfaceKHR m_Surface;
		VkSwapchainKHR m_SwapChain;
		std::vector<VkImage> m_SwapChainImages;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;
		std::vector<VkImageView> m_SwapChainImageViews;
	};

}