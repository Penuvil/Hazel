#pragma once

#if defined HZ_PLATFORM_WINDOWS
#if HZ_DYNAMIC_LINK
	#ifdef HZ_BUILD_DLL
		#define HAZEL_API __declspec(dllexport)
	#else
		#define HAZEL_API __declspec(dllimport)
	#endif
#else
    #define HAZEL_API
#endif
#define DEBUG_BREAK __debugbreak();
#elif defined HZ_PLATFORM_LINUX
#if HZ_DYNAMIC_LINK
	#ifdef HZ_BUILD_DLL
		#define HAZEL_API __attribute__((visibility("default")))
	#else
		#define HAZEL_API
	#endif
#else
    #define HAZEL_API
#endif
#include <csignal>
#define DEBUG_BREAK raise(SIGTRAP);
#else
	#error Hazel only supports Windows or Linux!
#endif


#ifdef HZ_DEBUG
	#define HZ_ENABLE_ASSERTS
#endif

#ifdef HZ_ENABLE_ASSERTS
	#define HZ_ASSERT(x, ...) { if(!(x)) { HZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); DEBUG_BREAK } }
	#define HZ_CORE_ASSERT(x, ...) { if(!(x)) { HZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); DEBUG_BREAK } }
#else
	#define HZ_ASSERT(x, ...)
	#define HZ_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define HZ_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)