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
		inline const VkRenderPass* GetRenderPass() { return &m_RenderPass; }
		inline const VulkanUtility::QueueFamilyIndices* GetQueueFamilyIndices() { return &m_QueueFamilyIndices; }
		inline const VkDescriptorPool* GetDescriptorPool() { return &m_DescriptorPool; }
	private:
		void Init();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateRenderPass();
		void CreateDescriptorPool();
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& supportedFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& supportedPresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		
	private:
		GLFWwindow* m_WindowHandle;
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_Device;
		VkSurfaceKHR m_Surface;
		VulkanUtility::QueueFamilyIndices m_QueueFamilyIndices;
		VkSwapchainKHR m_SwapChain;
		std::vector<VkImage> m_SwapChainImages;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;
		std::vector<VkImageView> m_SwapChainImageViews;
		VkRenderPass m_RenderPass;
		VkDescriptorPool m_DescriptorPool;
	};

}