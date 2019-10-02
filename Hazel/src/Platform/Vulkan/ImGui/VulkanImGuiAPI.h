#pragma once

#include "Hazel/ImGui/ImGuiAPI.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"

#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_vulkan.h"

#include <vulkan/vulkan.h>

namespace Hazel {

	class VulkanImGuiAPI : public ImGuiAPI
	{
	public:
		VulkanImGuiAPI();
		~VulkanImGuiAPI() = default;

		void CreateRenderPass();

		// Inherited via ImGuiAPI
		virtual void Init() override;
		void UploadFonts();
		virtual void Shutdown() override;
		virtual void OnImGuiRender() override;

		virtual void Begin() override;
		virtual void End() override;

	private:
		float m_Time = 0.0f;
		VkRenderPass m_RenderPass;
		std::vector<VkCommandBuffer>* m_CommandBuffers;
	};
}