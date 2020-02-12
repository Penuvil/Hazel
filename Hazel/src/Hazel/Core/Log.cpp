#include "hzpch.h"
#include "Hazel/Core/Log.h"

#ifdef HZ_PLATFORM_ANDROID
#include <spdlog/sinks/android_sink.h>
#else
#include <spdlog/sinks/stdout_color_sinks.h>
#endif

namespace Hazel {

	Ref<spdlog::logger> Log::s_CoreLogger;
	Ref<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
#ifdef HZ_PLATFORM_ANDROID
		s_CoreLogger = spdlog::android_logger_mt("HAZEL");

		s_ClientLogger = spdlog::android_logger_mt("APP");
#else
		s_CoreLogger = spdlog::stdout_color_mt("HAZEL");

		s_ClientLogger = spdlog::stdout_color_mt("APP");
#endif
		s_CoreLogger->set_level(spdlog::level::trace);
		
		s_ClientLogger->set_level(spdlog::level::trace);
	}

}
