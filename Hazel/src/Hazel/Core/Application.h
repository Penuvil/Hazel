#pragma once
#include <chrono>

#include "Hazel/Core/Core.h"

#include "Hazel/Core/Window.h"
#include "Hazel/Core/LayerStack.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Renderer/GraphicsContext.h"

#include "Hazel/Core/Timestep.h"

#include "Hazel/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Hazel {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }
#ifdef HZ_PLATFORM_ANDROID
		static void HandleAppCmd(struct android_app* app, int32_t cmd);
#endif
#ifdef HZ_PLATFORM_ANDROID
	public:
		struct AndroidAppState {
			struct android_app* androidApp;
			bool displayReady = false;
			EGLDisplay display;
			EGLSurface surface;
			EGLint displayWidth;
			EGLint displayHeight;
			Scope<GraphicsContext> graphicsContext;
		};
		static Ref<AndroidAppState> s_AndroidAppState;
#endif
	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		std::chrono::steady_clock::time_point m_LastFrameTime;
	private:
		static Application* s_Instance;
#ifdef HZ_PLATFORM_ANDROID
		friend void ::android_main(android_app* state);
#else
		friend int ::main(int argc, char** argv);
#endif
	};

	// To be defined in CLIENT
	Application* CreateApplication();


}