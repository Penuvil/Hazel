#include "hzpch.h"

#include "VulkanImGuiAPI.h"
#include "Platform/Vulkan/VulkanContext.h"

#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_vulkan.h"

#include "Hazel/Application.h"

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

		ImGui_ImplVulkan_Init(&imGUiImplVulkanInitInfo, *vulkanContext->GetSwapChain()->GetRenderPass());
		UploadFonts();
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
//		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),);

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	

}
