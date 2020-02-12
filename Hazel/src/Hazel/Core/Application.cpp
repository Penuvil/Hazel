#include "hzpch.h"
#include "Hazel/Core/Application.h"

#include "Hazel/Core/Log.h"

#include "Hazel/Renderer/Renderer.h"

#include "Hazel/Core/Input.h"

#ifndef HZ_PLATFORM_ANDROID
#include <glfw/glfw3.h>
#else
#include "Hazel/Renderer/GraphicsContext.h"
#endif

namespace Hazel {

	Application* Application::s_Instance = nullptr;

#ifdef HZ_PLATFORM_ANDROID
	Ref<Application::AndroidAppState> Application::s_AndroidAppState = CreateRef<Application::AndroidAppState>();
#endif

	Application::Application()
	{
		HZ_PROFILE_FUNCTION();

		HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
#ifdef HZ_PLATFORM_ANDROID
//		int ident;
		int events;
		struct android_poll_source* source;
		s_AndroidAppState->androidApp->onAppCmd = HandleAppCmd;
		while (!s_AndroidAppState->displayReady)
		{
            while (ALooper_pollAll(0, nullptr, &events, (void **) &source) >= 0)
            {
                if (source != nullptr) source->process(s_AndroidAppState->androidApp, source);
            }
        }
#endif

		m_Window = Window::Create();
		m_Window->SetEventCallback(HZ_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();



		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		HZ_PROFILE_FUNCTION();

		Renderer::Shutdown();
	}

	void Application::PushLayer(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		HZ_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(HZ_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			(*it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{
		HZ_PROFILE_FUNCTION();
#ifdef HZ_PLATFORM_ANDROID
		WindowResizeEvent event(s_AndroidAppState->displayWidth, s_AndroidAppState->displayHeight);
		Application::Get().OnEvent(event);
#endif
		while (m_Running)
		{
			HZ_PROFILE_SCOPE("RunLoop");

			auto time = std::chrono::steady_clock::now();
			Timestep timestep = { time - m_LastFrameTime };
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				{
					HZ_PROFILE_SCOPE("LayerStack OnUpdate");

					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}

//				m_ImGuiLayer->Begin();
//				{
//					HZ_PROFILE_SCOPE("LayerStack OnImGuiRender");
//
//					for (Layer* layer : m_LayerStack)
//						layer->OnImGuiRender();
//				}
//				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		HZ_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

#ifdef HZ_PLATFORM_ANDROID
	void Application::HandleAppCmd(android_app* app, int32_t cmd)
	{
		switch (cmd) {
			case APP_CMD_INIT_WINDOW:
				s_AndroidAppState->graphicsContext = GraphicsContext::Create(app->window);
				s_AndroidAppState->graphicsContext->Init();
				break;
		    case APP_CMD_WINDOW_RESIZED:
		        EGLDisplay display = eglGetCurrentDisplay();
		        EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
                eglQuerySurface(display, surface, EGL_WIDTH, &s_AndroidAppState->displayWidth);
                eglQuerySurface(display, surface, EGL_HEIGHT, &s_AndroidAppState->displayHeight);
                WindowResizeEvent event(s_AndroidAppState->displayWidth, s_AndroidAppState->displayHeight);
                Application::Get().OnEvent(event);
		}
	}
#endif
}
