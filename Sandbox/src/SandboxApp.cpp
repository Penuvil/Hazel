#include <Hazel.h>
#include <Hazel/Core/EntryPoint.h>

#include "Sandbox2D.h"

#include "ExampleLayer.h"

class Sandbox : public Hazel::Application
{
public:
	Sandbox(Hazel::RendererAPI::API api)
		:Hazel::Application(api)
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{
	}
};

Hazel::Application* Hazel::CreateApplication()
{
	return new Sandbox(Hazel::RendererAPI::API::Vulkan);
}