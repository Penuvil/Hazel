#pragma once

namespace Hazel {

	class ImGuiAPI
	{
	public:
		
		virtual ~ImGuiAPI() = default;

		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void OnImGuiRender() = 0;
		virtual void Begin() = 0;
		virtual void End() = 0;
	};
}
