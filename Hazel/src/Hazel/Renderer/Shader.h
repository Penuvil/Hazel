#pragma once

#include "Buffer.h"

#include <string>
#include <unordered_map>

namespace Hazel {

	class Shader
	{
	public:
		virtual ~Shader();

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const std::string& GetName() const = 0;

		virtual void SetUniformBuffers(const Ref<std::unordered_map<std::string, Ref<UniformBuffer>>> buffers) = 0;
		virtual inline Ref<UniformBuffer> GetUniformBuffer(const std::string& name) const = 0;

		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

	};

	class ShaderLibrary
	{
	public:
		ShaderLibrary();

		void Add(const std::string& name, const Ref<Shader>& shader);
		void Add(const Ref<Shader>& shader);
		void AddUniformBuffer(const Ref<UniformBuffer>& buffer);

		Ref<Shader> Load(const std::string& filepath);
		Ref<Shader> Load(const std::string& name, const std::string& filepath);

		Ref<Shader> Get(const std::string& name);
		Ref<UniformBuffer> GetUniformBuffer(const std::string& name);

		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
		Ref<std::unordered_map<std::string, Ref<UniformBuffer>>> m_UniformBuffers;
	};

}