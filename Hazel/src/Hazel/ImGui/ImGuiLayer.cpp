#include "hzpch.h"
#include "Hazel/ImGui/ImGuiLayer.h"

#include "Hazel/Core/Application.h"
#include "Hazel/Renderer/RendererAPI.h"
#include "Platform/OpenGL/ImGui/OpenGLImGuiAPI.h"
#include "Platform/Vulkan/ImGui/VulkanImGuiAPI.h"
#include "imgui.h"


namespace Hazel {

	ImGuiAPI* ImGuiLayer::s_ImGuiAPI = nullptr;

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			s_ImGuiAPI = new OpenGLImGuiAPI();
			break;
		case RendererAPI::API::Vulkan:
			s_ImGuiAPI = new VulkanImGuiAPI();
		}
	}

	void ImGuiLayer::OnAttach()
	{
		HZ_PROFILE_FUNCTION();
		s_ImGuiAPI->Init();
		}

	void ImGuiLayer::OnDetach()
	{
		HZ_PROFILE_FUNCTION();

		s_ImGuiAPI->Shutdown();
	}
	
	void ImGuiLayer::Begin()
	{
		HZ_PROFILE_FUNCTION();

		s_ImGuiAPI->Begin();
	}

	void ImGuiLayer::End()
	{
		HZ_PROFILE_FUNCTION();

		s_ImGuiAPI->End();
	}
}