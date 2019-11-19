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
		imGUiImplVulkanInitInfo.MinImageCount = 2;
		imGUiImplVulkanInitInfo.ImageCount = vulkanContext->GetSwapChain()->GetSwapImages()->size();
		imGUiImplVulkanInitInfo.CheckVkResultFn = CheckVkResult;

		ImGui_ImplVulkan_Init(&imGUiImplVulkanInitInfo, vulkanContext->GetSwapChain()->GetRenderPass("NoClear"));
		UploadFonts();
//		CreateRenderPass();
	}

	void VulkanImGuiAPI::UploadFonts()
	{
		VkCommandBuffer commandBuffer = VulkanUtility::BeginTransientCommand();

		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

		VulkanUtility::EndTransientCommand(commandBuffer);

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	void VulkanImGuiAPI::Shutdown()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void VulkanImGuiAPI::OnImGuiRender()
	{

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
		renderPassBeginInfo.renderPass = vulkanSwapChain->GetRenderPass("NoClear");
		renderPassBeginInfo.framebuffer = vulkanSwapChain->GetFramebuffers()->at(frameInfo->imageIndex);
		renderPassBeginInfo.renderArea.offset = { 0,0 };
		renderPassBeginInfo.renderArea.extent = *vulkanSwapChain->GetExtent2D();
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(frameInfo->clearColors.size());
		renderPassBeginInfo.pClearValues = frameInfo->clearColors.data();

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

		vkWaitForFences(*VulkanContext::GetContext()->GetDevice(), 1, frameInfo->inFlightFence, VK_TRUE, UINT64_MAX);
//		vkResetFences(*VulkanContext::GetContext()->GetDevice(), 1, frameInfo->inFlightFence);

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
		
	}

	

}
