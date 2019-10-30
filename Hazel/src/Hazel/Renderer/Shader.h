#pragma once

#include "Buffer.h"

#include <string>
#include <unordered_map>

namespace Hazel {

	class Shader : public std::enable_shared_from_this<Shader>
	{
	public:
		virtual ~Shader();

		virtual void Bind() = 0;
		virtual void Unbind() const = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& filepath, const BufferLayout& vertexBufferLayout);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc, const BufferLayout& vertexBufferLayout);
	protected:
		std::string ReadFile(const std::string& filepath);
	};

	class ShaderLibrary
	{
	public:
		ShaderLibrary();

		void Add(const std::string& name, const Ref<Shader>& shader);
		void Add(const Ref<Shader>& shader);

		Ref<Shader> Load(const std::string& filepath, const BufferLayout& vertexBufferLayout);
		Ref<Shader> Load(const std::string& name, const std::string& filepath, const BufferLayout& vertexBufferLayout);

		Ref<Shader> Get(const std::string& name);

		static ShaderLibrary* GetInstance() { return s_ShaderLibrary; }

		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;

		static ShaderLibrary* s_ShaderLibrary;
	};

}