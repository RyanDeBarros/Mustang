#include "UniformLexicon.h"

#include "UniformLexiconRegistry.h"
#include "ShaderRegistry.h"

UniformLexicon::UniformLexicon(const std::unordered_map<std::string, Uniform>& uniform_set)
	: m_Uniforms(uniform_set)
{
}

UniformLexicon::UniformLexicon(const UniformLexicon& other)
	: m_Uniforms(other.m_Uniforms)
{
}

UniformLexicon::UniformLexicon(UniformLexicon&& other) noexcept
	: m_Uniforms(std::move(other.m_Uniforms))
{
}

UniformLexicon& UniformLexicon::operator=(const UniformLexicon& other)
{
	m_Uniforms = other.m_Uniforms;
	return *this;
}

UniformLexicon& UniformLexicon::operator=(UniformLexicon&& other) noexcept
{
	m_Uniforms = std::move(other.m_Uniforms);
	return *this;
}

UniformLexicon::~UniformLexicon()
{
}

void UniformLexicon::MergeLexicon(UniformLexiconHandle lexicon_handle)
{
	if (lexicon_handle != 0)
	{
		UniformLexicon lexicon = *UniformLexiconRegistry::Get(lexicon_handle);
		m_Uniforms.insert(lexicon.m_Uniforms.begin(), lexicon.m_Uniforms.end());
	}
}

bool UniformLexicon::Shares(const UniformLexicon& lexicon)
{
	auto ait = m_Uniforms.begin();
	auto bit = lexicon.m_Uniforms.begin();
	while (ait != m_Uniforms.end() && bit != lexicon.m_Uniforms.end())
	{
		const std::string& akey = ait->first;
		const std::string& bkey = bit->first;
		if (akey < bkey)
			ait++;
		else if (akey > bkey)
			bit++;
		else if (ait->second != bit->second)
			return false;
	}
	return true;
}

bool UniformLexicon::Shares(UniformLexiconHandle lexicon_handle)
{
	if (lexicon_handle != 0)
	{
		auto lexicon = UniformLexiconRegistry::Get(lexicon_handle);
		if (lexicon)
		{
			auto ait = m_Uniforms.begin();
			auto bit = lexicon->m_Uniforms.begin();
			while (ait != m_Uniforms.end() && bit != lexicon->m_Uniforms.end())
			{
				const std::string& akey = ait->first;
				const std::string& bkey = bit->first;
				if (akey < bkey)
					ait++;
				else if (akey > bkey)
					bit++;
				else if (ait->second != bit->second)
					return false;
			}
		}
	}
	return true;
}

void UniformLexicon::OnApply(ShaderHandle shader) const
{
	for (const auto& [name, uniform] : m_Uniforms)
	{
		switch (uniform.index())
		{
		case 0:
			ShaderRegistry::SetUniform1i(shader, name.c_str(), std::get<GLint>(uniform));
			break;
		case 1:
			ShaderRegistry::SetUniform2iv(shader, name.c_str(), &std::get<glm::ivec2>(uniform)[0]);
			break;
		case 2:
			ShaderRegistry::SetUniform3iv(shader, name.c_str(), &std::get<glm::ivec3>(uniform)[0]);
			break;
		case 3:
			ShaderRegistry::SetUniform4iv(shader, name.c_str(), &std::get<glm::ivec4>(uniform)[0]);
			break;
		case 4:
			ShaderRegistry::SetUniform1ui(shader, name.c_str(), std::get<GLuint>(uniform));
			break;
		case 5:
			ShaderRegistry::SetUniform2uiv(shader, name.c_str(), &std::get<glm::uvec2>(uniform)[0]);
			break;
		case 6:
			ShaderRegistry::SetUniform3uiv(shader, name.c_str(), &std::get<glm::uvec3>(uniform)[0]);
			break;
		case 7:
			ShaderRegistry::SetUniform4uiv(shader, name.c_str(), &std::get<glm::uvec4>(uniform)[0]);
			break;
		case 8:
			ShaderRegistry::SetUniform1f(shader, name.c_str(), std::get<GLfloat>(uniform));
			break;
		case 9:
			ShaderRegistry::SetUniform2fv(shader, name.c_str(), &std::get<glm::vec2>(uniform)[0]);
			break;
		case 10:
			ShaderRegistry::SetUniform3fv(shader, name.c_str(), &std::get<glm::vec3>(uniform)[0]);
			break;
		case 11:
			ShaderRegistry::SetUniform4fv(shader, name.c_str(), &std::get<glm::vec4>(uniform)[0]);
			break;
		case 12:
			ShaderRegistry::SetUniformMatrix2fv(shader, name.c_str(), &std::get<glm::mat2>(uniform)[0][0]);
			break;
		case 13:
			ShaderRegistry::SetUniformMatrix3fv(shader, name.c_str(), &std::get<glm::mat3>(uniform)[0][0]);
			break;
		case 14:
			ShaderRegistry::SetUniformMatrix4fv(shader, name.c_str(), &std::get<glm::mat4>(uniform)[0][0]);
			break;
		}
	}
}

Uniform const* UniformLexicon::GetValue(const std::string& name) const
{
	auto it = m_Uniforms.find(name);
	if (it != m_Uniforms.end())
		return &it->second;
	else
		return nullptr;
}

bool UniformLexicon::SetValue(const std::string& name, const Uniform& uniform) {
	auto it = m_Uniforms.find(name);
	if (it != m_Uniforms.end())
	{
		if (it->second != uniform)
		{
			it->second = uniform;
			return true;
		}
	}
	return false;
}

bool UniformLexicon::DefineNewValue(const std::string& name, const Uniform& uniform)
{
	auto it = m_Uniforms.find(name);
	if (it == m_Uniforms.end())
	{
		m_Uniforms.insert({ name, uniform });
		return true;
	}
	return false;
}

void UniformLexicon::Clear()
{
	m_Uniforms.clear();
}
