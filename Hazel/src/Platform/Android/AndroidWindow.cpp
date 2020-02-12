#include "hzpch.h"

#ifdef HZ_PLATFORM_ANDROID

#include "AndroidWindow.h"

namespace Hazel {

	AndroidWindow::AndroidWindow()
	{
		Init();
	}

	AndroidWindow::~AndroidWindow()
	{
	}

	void AndroidWindow::OnUpdate()
	{
		HZ_PROFILE_FUNCTION();

		int events;
		struct android_poll_source* source;
		while (ALooper_pollAll(0, nullptr, &events, (void **) &source) >= 0)
        {
            if (source != nullptr) source->process(Application::s_AndroidAppState->androidApp, source);
        }

		Application::s_AndroidAppState->graphicsContext->SwapBuffers();
	}
	
	void AndroidWindow::SetVSync(bool enabled)
	{
	}
	
	bool AndroidWindow::IsVSync() const
	{
		return false;
	}

	void AndroidWindow::Init()
	{
		m_Data.Width = Application::s_AndroidAppState->displayWidth;
		m_Data.Height = Application::s_AndroidAppState->displayHeight;
	}
}
#endif