#include "hzpch.h"
#include "Hazel/Core/Input.h"

#ifdef HZ_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsInput.h"
#elif defined(HZ_PLATFORM_ANDROID)
	#include "Platform/Android/AndroidInput.h"
#endif

namespace Hazel {

	Scope<Input> Input::s_Instance = Input::Create();

	Scope<Input> Input::Create()
	{
	#ifdef HZ_PLATFORM_WINDOWS
		return CreateScope<WindowsInput>();
    #elif defined(HZ_PLATFORM_ANDROID)
		return CreateScope<AndroidInput>();
	#else
		HZ_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
	#endif
	}
} 