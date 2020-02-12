#include "hzpch.h"
#include "Hazel/Core/Application.h"
#include "Platform/OpenGL/OpenGLContext.h"

#ifdef HZ_PLATFORM_ANDROID
#include <EGL/egl.h>
#include <GLES3/gl32.h>
#else
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>
#endif // !HZ_PLATFORM_ANDROID


namespace Hazel {
#ifdef HZ_PLATFORM_ANDROID
	OpenGLContext::OpenGLContext(ANativeWindow* windowHandle)
#else
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
#endif
		: m_WindowHandle(windowHandle)
	{
		HZ_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		HZ_PROFILE_FUNCTION();
#ifdef HZ_PLATFORM_ANDROID
		const EGLint attribs[] = {
			EGL_SURFACE_TYPE,
			EGL_WINDOW_BIT,
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
			EGL_DEPTH_SIZE, 24,
			EGL_NONE
		};
		EGLint w, h, dummy, format;
		EGLint numConfigs;
		EGLConfig config;
		EGLSurface surface;
		EGLContext context;

		EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

		eglInitialize(display, 0, 0);

		eglChooseConfig(display, attribs, &config, 1, &numConfigs);

		eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

		ANativeWindow_setBuffersGeometry(Application::s_AndroidAppState->androidApp->window, 0, 0, format);

		surface = eglCreateWindowSurface(display, config, Application::s_AndroidAppState->androidApp->window, NULL);
		const EGLint attributes[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
		context = eglCreateContext(display, config, NULL, attributes );

		HZ_CORE_ASSERT(eglMakeCurrent(display, surface, surface, context) != EGL_FALSE, "Unable to eglMakeCurrent!");

		eglQuerySurface(display, surface, EGL_WIDTH, &w);
		eglQuerySurface(display, surface, EGL_HEIGHT, &h);

		Application::s_AndroidAppState->display = display;
		Application::s_AndroidAppState->surface = surface;
		Application::s_AndroidAppState->displayWidth = w;
		Application::s_AndroidAppState->displayHeight = h;
		Application::s_AndroidAppState->displayReady = true;

#else
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		HZ_CORE_ASSERT(status, "Failed to initialize Glad!");
#endif

		HZ_CORE_INFO("OpenGL Info:");
		HZ_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		HZ_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		HZ_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

	#ifdef HZ_ENABLE_ASSERTS
		int versionMajor;
		int versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

	#ifdef HZ_PLATFORM_ANDROID
		HZ_CORE_ASSERT(versionMajor > 3 || (versionMajor == 3 && versionMinor >= 2), "Hazel requires at least OprnGL ES version 3.2!");
	#else
		HZ_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Hazel requires at least OpenGL version 4.5!");
	#endif
	#endif
	}

	void OpenGLContext::SwapBuffers()
	{
		HZ_PROFILE_FUNCTION();
#ifdef HZ_PLATFORM_ANDROID
		eglSwapBuffers(Application::s_AndroidAppState->display, Application::s_AndroidAppState->surface);
#else
		glfwSwapBuffers(m_WindowHandle);
#endif
	}

}