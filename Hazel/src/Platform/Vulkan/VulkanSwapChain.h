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
		inline const VkSwapchainKHR* GetSwapChain() { return &m_SwapChain; }
		inline const VkExtent2D* GetExtent2D() { return &m_SwapChainExtent; }
		inline const VkRenderPass* GetRenderPass() { return &m_RenderPass; }
		inline const VulkanUtility::QueueFamilyIndices* GetQueueFamilyIndices() { return &m_QueueFamilyIndices; }
		inline const VkDescriptorPool* GetDescriptorPool() { return &m_DescriptorPool; }
		inline VkDescriptorSetLayout* GetDescriptorSetLayout() { return &m_DescriptorSetLayout; }
		inline std::vector<VkFramebuffer>* GetFramebuffers() { return &m_Framebuffers;  }
		inline std::vector<VkCommandBuffer>* GetCommandBuffers() { return &m_CommandBuffers; }
		inline std::vector<VkCommandBuffer>* GetImGuiCommandBuffer() { return &m_ImGuiCommandBuffers; }
		inline const VkFormat* GetSwapChainImageFormat() { return &m_SwapChainImageFormat; }
		inline const std::vector<VkImage>* GetSwapImages() { return &m_SwapChainImages; }
	private:
		void Init();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateDescriptorPool();
		void CreateDescriptorSetLayout();
		void AllocateCommandBuffers();
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
		std::vector<VkFramebuffer> m_Framebuffers;
		VkDescriptorPool m_DescriptorPool;
		VkDescriptorSetLayout m_DescriptorSetLayout;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		std::vector<VkCommandBuffer> m_ImGuiCommandBuffers;
	};

}