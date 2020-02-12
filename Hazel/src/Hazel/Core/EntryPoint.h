#pragma once
#include "Hazel/Core/Core.h"

extern Hazel::Application* Hazel::CreateApplication();

#ifdef HZ_PLATFORM_WINDOWS
int main(int argc, char** argv)
#elif defined(HZ_PLATFORM_ANDROID)
void android_main(android_app* state)
#endif
{
	Hazel::Log::Init();

#ifdef HZ_PLATFORM_ANDROID
    Hazel::Application::s_AndroidAppState->androidApp = state;
#endif

	HZ_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Startup.json");
	auto app = Hazel::CreateApplication();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile-Runtime.json");
	app->Run();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Shutdown.json");
	delete app;
	HZ_PROFILE_END_SESSION();
}

