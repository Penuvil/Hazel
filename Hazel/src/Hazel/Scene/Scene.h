#pragma once

#include "entt.hpp"


#include "Hazel/Core/Timestep.h"
#include "Hazel/Renderer/Framebuffer.h"

namespace Hazel {

	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());

		void OnUpdate(Timestep ts, Ref<Framebuffer> framebuffer);
	private:
		entt::registry m_Registry;

		friend class Entity;
	};

}