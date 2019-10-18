#include "hzpch.h"

#include "VulkanImGuiAPI.h"
#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"



#include "Hazel/Core/Application.h"

static void CheckVkResult(VkResult result)
{
	HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to init ImGui! " + result);
}

namespace Hazel {

	VulkanImGuiAPI::VulkanImGuiAPI()
	{
	}

	void VulkanImGuiAPI::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.flags = 0;
		colorAttachment.format = *VulkanContext::GetContext()->GetSwapChain()->GetSwapChainImageFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.flags = 0;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentRef;
		subpassDescription.pResolveAttachments = nullptr;
		subpassDescription.pDepthStencilAttachment = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;

		VkSubpassDependency subpassDependency = {};
		subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependency.dstSubpass = 0;
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.srcAccessMask = 0;
		subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpassDependency.dependencyFlags = 0;

		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = NULL;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &colorAttachment;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &subpassDependency;

		VkResult result = vkCreateRenderPass(*VulkanContext::GetContext()->GetDevice(), &renderPassCreateInfo, nullptr, &m_RenderPass);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create render pass! " + result);
	}

	void VulkanImGuiAPI::Init()
	{
		m_CommandBuffers = VulkanContext::GetContext()->GetSwapChain()->GetImGuiCommandBuffer();
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// Setup Platform/Renderer bindings
		VulkanContext* vulkanContext = VulkanContext::GetContext();

		ImGui_ImplGlfw_InitForVulkan(window, true);

		ImGui_ImplVulkan_InitInfo imGUiImplVulkanInitInfo = {};
		imGUiImplVulkanInitInfo.Instance = *vulkanContext->GetInstance();
		imGUiImplVulkanInitInfo.PhysicalDevice = *vulkanContext->GetPhysicalDevice();
		imGUiImplVulkanInitInfo.Device = *vulkanContext->GetDevice();
		imGUiImplVulkanInitInfo.QueueFamily = vulkanContext->GetSwapChain()->GetQueueFamilyIndices()->graphicsFamily.value();
		imGUiImplVulkanInitInfo.Queue = *vulkanContext->GetGraphicsQueue();
		imGUiImplVulkanInitInfo.PipelineCache = VK_NULL_HANDLE;
		imGUiImplVulkanInitInfo.DescriptorPool = *vulkanContext->GetSwapChain()->GetDescriptorPool();
		imGUiImplVulkanInitInfo.Allocator = nullptr;
		imGUiImplVulkanInitInfo.CheckVkResultFn = CheckVkResult;

		ImGui_ImplVulkan_Init(&imGUiImplVulkanInitInfo, vulkanContext->GetSwapChain()->GetRenderPass("NoClear"));
		UploadFonts();
		CreateRenderPass();
	}

	void VulkanImGuiAPI::UploadFonts()
	{
		VkResult result;
		VkDevice* device = VulkanContext::GetContext()->GetDevice();
		VkCommandPool* commandPool = VulkanContext::GetContext()->GetCommandPool();
		VkCommandBuffer commandBuffer;
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = NULL;
		commandBufferAllocateInfo.commandPool = *commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;

		result = vkAllocateCommandBuffers(*device, &commandBufferAllocateInfo, &commandBuffer);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate command buffer! ", +result);
		
		VkCommandBufferBeginInfo commandBuffreBeginInfo = {};
		commandBuffreBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBuffreBeginInfo.pNext = NULL;
		commandBuffreBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		commandBuffreBeginInfo.pInheritanceInfo = VK_NULL_HANDLE;

		result = vkBeginCommandBuffer(commandBuffer, &commandBuffreBeginInfo);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to begin command buffer! " + result);

		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = NULL;
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;

		result = vkEndCommandBuffer(commandBuffer);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to end command buffer!" + result);

		VkQueue* graphicsQueue = VulkanContext::GetContext()->GetGraphicsQueue();
		result = vkQueueSubmit(*graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to subit command buffers to queue! " + result);
		vkQueueWaitIdle(*graphicsQueue);

		vkFreeCommandBuffers(*device, *commandPool, 1, &commandBuffer);
		ImGui_ImplVulkan_InvalidateFontUploadObjects();
	}

	void VulkanImGuiAPI::Shutdown()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void VulkanImGuiAPI::OnImGuiRender()
	{
		static bool show = true;
		ImGui::ShowDemoWindow(&show);
	}

	void VulkanImGuiAPI::Begin()
	{
		vkWaitForFences(*VulkanContext::GetContext()->GetDevice(), 1, VulkanRendererAPI::GetFrame()->inFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(*VulkanContext::GetContext()->GetDevice(), 1, VulkanRendererAPI::GetFrame()->inFlightFence);
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void VulkanImGuiAPI::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		// Rendering
		ImGui::Render();

		VkResult result;
		VulkanContext* vulkanContext = VulkanContext::GetContext();
		Ref<VulkanSwapChain> vulkanSwapChain = vulkanContext->GetSwapChain();
		Ref<VulkanRendererAPI::FrameInfo> frameInfo = VulkanRendererAPI::GetFrame();

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = NULL;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		result = vkBeginCommandBuffer(m_CommandBuffers->at(frameInfo->imageIndex), &commandBufferBeginInfo);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to begin command buffer! " + result);

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = NULL;
		renderPassBeginInfo.renderPass = m_RenderPass;
		renderPassBeginInfo.framebuffer = vulkanSwapChain->GetFramebuffers()->at(frameInfo->imageIndex);
		renderPassBeginInfo.renderArea.offset = { 0,0 };
		renderPassBeginInfo.renderArea.extent = *vulkanSwapChain->GetExtent2D();
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = frameInfo->clearColor;

		vkCmdBeginRenderPass(m_CommandBuffers->at(frameInfo->imageIndex), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_CommandBuffers->at(frameInfo->imageIndex));

		vkCmdEndRenderPass(m_CommandBuffers->at(frameInfo->imageIndex));

		result = vkEndCommandBuffer(m_CommandBuffers->at(frameInfo->imageIndex));
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to en command buffer! " + result);

		VkSemaphore waitSemaphores[] = { *frameInfo->layerCompleteSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore signalSemiphores[] = { *frameInfo->renderFinishedSemaphore };

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = NULL;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffers->at(frameInfo->imageIndex);
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemiphores;
		
		result = vkQueueSubmit(*vulkanContext->GetGraphicsQueue(), 1, &submitInfo, *frameInfo->inFlightFence);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to submit command queue!");

		const VkSwapchainKHR* swapChain = vulkanSwapChain->GetSwapChain();

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = NULL;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemiphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChain;
		presentInfo.pImageIndices = &frameInfo->imageIndex;
		presentInfo.pResults = nullptr;

		vkQueuePresentKHR(*vulkanContext->GetPresentQueue(), &presentInfo);


		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	

}
