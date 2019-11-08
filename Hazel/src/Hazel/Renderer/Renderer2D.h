#pragma once

#include "OrthographicCamera.h"

#include "Texture.h"

namespace Hazel {

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Ref<OrthographicCamera> camera);
		static void EndScene();
		static void BeginRender();
		static void EndRender();

		// Primitives

		static void DrawQuad(const uint32_t instanceID, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const uint32_t instanceID, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const uint32_t instanceID, const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture);
		static void DrawQuad(const uint32_t instanceID, const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture);
	};

}