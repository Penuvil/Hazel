#pragma once

#include "Hazel/Core/Layer.h"
#include "Hazel/ImGui/ImGuiAPI.h"
#include "Hazel/Core/Layer.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"

namespace Hazel {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void Begin();
		virtual void End();
	private:
		float m_Time = 0.0f;
		static ImGuiAPI* s_ImGuiAPI;
	};

}