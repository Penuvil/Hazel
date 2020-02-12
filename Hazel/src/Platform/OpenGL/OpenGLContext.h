#pragma once

#include "Hazel/Renderer/GraphicsContext.h"
#ifdef HZ_PLATFORM_ANDROID
struct ANativeWindow;
#else
struct GLFWwindow;
#endif

namespace Hazel {

	class OpenGLContext : public GraphicsContext
	{
	public:
#ifdef HZ_PLATFORM_ANDROID
		OpenGLContext(ANativeWindow* windowHandle);
#else
		OpenGLContext(GLFWwindow* windowHandle);
#endif

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
#ifdef HZ_PLATFORM_ANDROID
		ANativeWindow* m_WindowHandle;
#else
		GLFWwindow* m_WindowHandle;
#endif
	};

}