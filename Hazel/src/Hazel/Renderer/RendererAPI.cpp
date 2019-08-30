#include "hzpch.h"
#include "RendererAPI.h"

namespace Hazel {

	RendererAPI::API RendererAPI::s_API = RendererAPI::API::None;

	void RendererAPI::SetAPI(API api)
	{
		s_API = api;
	}

}