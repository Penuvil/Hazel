#pragma once

#ifdef HZ_PLATFORM_ANDROID
#include "Hazel/Core/Application.h"
#include "Hazel/Core/Window.h"
#include "Hazel/Renderer/GraphicsContext.h"

namespace Hazel {

	class AndroidWindow : public Window
	{
	public:
		AndroidWindow();
		virtual ~AndroidWindow();

		void OnUpdate() override;

		unsigned int GetWidth() const override { return ANativeWindow_getWidth(m_Window); }
		unsigned int GetHeight() const override { return ANativeWindow_getHeight(m_Window); }

		void SetEventCallback(const EventCallbackFn& callback) override { /*m_Data.EventCallback = callback;*/ }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		void* GetNativeWindow() const override { return m_Window; }
	private:
		void Init();

	private:
		ANativeWindow* m_Window;
		Scope<GraphicsContext> m_Context;
		struct WindowData
		{
			unsigned int Width, Height;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}
#endif