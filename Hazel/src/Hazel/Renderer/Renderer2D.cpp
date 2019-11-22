#include "hzpch.h"
#include "Hazel/Renderer/Renderer2D.h"

#include "Hazel/Renderer/VertexArray.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {

	struct Renderer2DStorage
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;
		Ref<OrthographicCamera> Camera;
	};

	static Renderer2DStorage* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DStorage();
		s_Data->QuadVertexArray = VertexArray::Create(3);

		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		Ref<VertexBuffer> squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		});
		s_Data->QuadVertexArray->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Ref<IndexBuffer> squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		s_Data->QuadVertexArray->SetIndexBuffer(squareIB);

		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl", squareVB->GetLayout());
//		s_Data->TextureShader->Bind();
//		s_Data->TextureShader->SetInt("u_Texture", 0);
	}

	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}

	void Renderer2D::BeginScene(const Ref<OrthographicCamera> camera)
	{
		RenderCommand::BeginScene();
		//std::dynamic_pointer_cast<OpenGLShader>(s_Data->FlatColorShader)->Bind();
		//std::dynamic_pointer_cast<OpenGLShader>(s_Data->FlatColorShader)->UploadUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		//std::dynamic_pointer_cast<OpenGLShader>(s_Data->FlatColorShader)->UploadUniformMat4("u_Transform", glm::mat4(1.0f));

		s_Data->Camera = camera;
		
//		s_Data->QuadVertexArray->GetUniformBuffer(0, "Matrices")->Bind();
//		s_Data->QuadVertexArray->GetUniformBuffer(0, "Matrices")->UpdateMat4("u_ViewProjection", camera->GetViewProjectionMatrix());
//		s_Data->QuadVertexArray->GetUniformBuffer(0, "Matrices")->UpdateMat4("u_Transform", glm::mat4(1.0f));
	}

	void Renderer2D::EndScene()
	{
		RenderCommand::EndScene();
	}

	void Renderer2D::BeginRender()
	{
		RenderCommand::BeginRender();
		s_Data->TextureShader->Bind();
		s_Data->TextureShader->SetInt("u_Texture", 0);
	}

	void Renderer2D::EndRender()
	{
		RenderCommand::EndRender();
	}

	void Renderer2D::DrawQuad(const uint32_t instanceID, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad( instanceID, { position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const uint32_t instanceID, const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		s_Data->WhiteTexture->Bind();

		s_Data->QuadVertexArray->GetUniformBuffer(instanceID, "Matrices")->Bind();
		s_Data->QuadVertexArray->GetUniformBuffer(instanceID, "Matrices")->UpdateMat4("u_ViewProjection", s_Data->Camera->GetViewProjectionMatrix());
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->QuadVertexArray->GetUniformBuffer(instanceID, "Matrices")->UpdateMat4("u_Transform", transform);
//		s_Data->TextureShader->SetMat4("u_Transform", transform);

//		std::dynamic_pointer_cast<OpenGLShader>(s_Data->FlatColorShader)->Bind();
//		std::dynamic_pointer_cast<Hazel::OpenGLShader>(s_Data->FlatColorShader)->UploadUniformFloat4("u_Color", color);
		s_Data->QuadVertexArray->GetUniformBuffer(instanceID, "Color")->Bind();
		s_Data->QuadVertexArray->GetUniformBuffer(instanceID, "Color")->UpdateFloat4("u_Color", color);

//		s_Data->TextureShader->SetFloat4("u_Color", color);
			
		s_Data->QuadVertexArray->Bind(instanceID);
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray, instanceID);
	}

	void Renderer2D::DrawQuad(const uint32_t instanceID, const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		DrawQuad(instanceID, { position.x, position.y, 0.0f }, size, texture);
	}

	void Renderer2D::DrawQuad(const uint32_t instanceID, const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		texture->Bind();
		s_Data->QuadVertexArray->GetUniformBuffer(instanceID, "Matrices")->Bind();
		s_Data->QuadVertexArray->GetUniformBuffer(instanceID, "Matrices")->UpdateMat4("u_ViewProjection", s_Data->Camera->GetViewProjectionMatrix());
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data->QuadVertexArray->GetUniformBuffer(instanceID, "Matrices")->UpdateMat4("u_Transform", transform);

		s_Data->QuadVertexArray->GetUniformBuffer(instanceID, "Color")->Bind();
		s_Data->QuadVertexArray->GetUniformBuffer(instanceID, "Color")->UpdateFloat4("u_Color", glm::vec4(1.0f));

		s_Data->QuadVertexArray->Bind(instanceID);
		RenderCommand::DrawIndexed(s_Data->QuadVertexArray, instanceID);
	}

}