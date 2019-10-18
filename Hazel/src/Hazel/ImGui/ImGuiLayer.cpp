#include "hzpch.h"
#include "ImGuiLayer.h"


#include "Hazel/Core/Application.h"
#include "Hazel/Renderer/RendererAPI.h"
#include "Platform/OpenGL/ImGui/OpenGLImGuiAPI.h"
#include "Platform/Vulkan/ImGui/VulkanImGuiAPI.h"
#include "imgui.h"


namespace Hazel {

	ImGuiAPI* ImGuiLayer::s_ImGuiAIP = nullptr;

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			s_ImGuiAIP = new OpenGLImGuiAPI();
			break;
		case RendererAPI::API::Vulkan:
			s_ImGuiAIP = new VulkanImGuiAPI();
		}
	}

	void ImGuiLayer::OnAttach()
	{
		s_ImGuiAIP->Init();
	}

	void ImGuiLayer::OnDetach()
	{
		s_ImGuiAIP->Shutdown();
	}
	
	void ImGuiLayer::Begin()
	{
		s_ImGuiAIP->Begin();
	}

	void ImGuiLayer::End()
	{
		s_ImGuiAIP->End();
	}

	void ImGuiLayer::OnImGuiRender()
	{
		s_ImGuiAIP->OnImGuiRender();
	}

}