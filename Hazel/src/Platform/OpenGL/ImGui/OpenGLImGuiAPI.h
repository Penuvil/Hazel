#pragma once

#include "Hazel/ImGui/ImGuiAPI.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"

namespace Hazel {

	class HAZEL_API OpenGLImGuiAPI : public ImGuiAPI
	{
	public:
		OpenGLImGuiAPI();
		~OpenGLImGuiAPI() = default;

		virtual void Init() override;
		virtual void Shutdown() override;
		virtual void OnImGuiRender() override;

		virtual void Begin() override;
		virtual void End() override;
	private:
		float m_Time = 0.0f;
	};

}