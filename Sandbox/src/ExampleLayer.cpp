#include "ExampleLayer.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

ExampleLayer::ExampleLayer() 
	: Layer("ExampleLayer"), m_CameraController(1280.0f / 720.0f)
{
	m_VertexArray = Hazel::VertexArray::Create(1);

	float vertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
		 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
		 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
	};

	Hazel::Ref<Hazel::VertexBuffer> vertexBuffer = Hazel::VertexBuffer::Create(vertices, sizeof(vertices));
	Hazel::BufferLayout layout = {
		{ Hazel::ShaderDataType::Float3, "a_Position" },
		{ Hazel::ShaderDataType::Float4, "a_Color" }
	};
	vertexBuffer->SetLayout(layout);
	m_VertexArray->AddVertexBuffer(vertexBuffer);

	uint32_t indices[3] = { 0, 1, 2 };
	Hazel::Ref<Hazel::IndexBuffer> indexBuffer = Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	m_VertexArray->SetIndexBuffer(indexBuffer);

	m_SquareVA = Hazel::VertexArray::Create(20 * 20 + 2);

	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};

	Hazel::Ref<Hazel::VertexBuffer> squareVB = Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
	squareVB->SetLayout({
		{ Hazel::ShaderDataType::Float3, "a_Position" },
		{ Hazel::ShaderDataType::Float2, "a_TexCoord" }
		});
	m_SquareVA->AddVertexBuffer(squareVB);

	uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	Hazel::Ref<Hazel::IndexBuffer> squareIB = Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
	m_SquareVA->SetIndexBuffer(squareIB);

	std::string vertexSrc = R"(
			#version 450
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			layout(binding = 0) uniform Matrices
			{
				mat4 u_ViewProjection;
				mat4 u_Transform;
			} mats;

			layout(location = 0) out vec3 v_Position;
			layout(location = 1) out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = mats.u_ViewProjection * mats.u_Transform * vec4(a_Position, 1.0);	
			}
		)";

	std::string fragmentSrc = R"(
			#version 450
			
			layout(location = 0) out vec4 color;

			layout(location = 0) in vec3 v_Position;
			layout(location = 1) in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

	m_Shader = Hazel::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc, vertexBuffer->GetLayout());
	m_ShaderLibrary.Add(m_Shader);

	std::string flatColorShaderVertexSrc = R"(
			#version 450
			
			layout(location = 0) in vec3 a_Position;

			layout(binding = 0) uniform Matrices
			{
				mat4 u_ViewProjection;
				mat4 u_Transform;
			} mats;

			layout(location = 0) out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;

				gl_Position = mats.u_ViewProjection * mats.u_Transform * vec4(a_Position, 1.0);
			}
		)";

	std::string flatColorShaderFragmentSrc = R"(
			#version 450
			
			layout(location = 0) out vec4 color;

			layout(location = 0) in vec3 v_Position;
			
			layout(binding = 1) uniform Color
			{
				vec3 u_Color;
			} col;		

			void main()
			{
				color = vec4(col.u_Color, 1.0);
			}
		)";

	m_FlatColorShader = Hazel::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc, squareVB->GetLayout());
	m_ShaderLibrary.Add(m_FlatColorShader);

	auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl", squareVB->GetLayout());

	m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
	m_ChernoLogoTexture = Hazel::Texture2D::Create("assets/textures/ChernoLogo.png");

}

void ExampleLayer::OnAttach()
{
}

void ExampleLayer::OnDetach()
{
}

void ExampleLayer::OnUpdate(Hazel::Timestep ts) 
{
	// Update
	m_CameraController.OnUpdate(ts);

	Hazel::Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Hazel::Renderer::Clear();

	Hazel::Renderer::BeginScene(m_CameraController.GetCamera());

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

	uint32_t instance = 0;

	m_FlatColorShader->Bind();
	m_FlatColorShader->SetFloat3("u_Color", m_SquareColor);


	for (int y = 0; y < 20; y++)
	{
		for (int x = 0; x < 20; x++)
		{
			glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
			Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, instance, glm::vec4(m_SquareColor, 1.0f), transform);
			instance++;
		}
	}

	auto textureShader = m_ShaderLibrary.Get("Texture");
	textureShader->SetInt("u_Texture", 0);


	//		m_Texture->Bind();
	Hazel::Renderer::Submit(textureShader, m_SquareVA, m_Texture, instance, glm::vec4(m_SquareColor, 1.0f), glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	//		m_ChernoLogoTexture->Bind();
	instance++;
	Hazel::Renderer::Submit(textureShader, m_SquareVA, m_ChernoLogoTexture, instance, glm::vec4(m_SquareColor, 1.0f), glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	// Triangle

	Hazel::Renderer::EndScene();
}

void ExampleLayer::OnImGuiRender() 
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void ExampleLayer::OnEvent(Hazel::Event& e) 
{
	m_CameraController.OnEvent(e);
}
