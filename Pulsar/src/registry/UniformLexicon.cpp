#include "UniformLexicon.h"

#include "Shader.h"
#include "render/Renderer.h"
#include "Macros.h"

#if !PULSAR_IGNORE_WARNINGS_NULL_UNIFORM_LEXICON
#include "Logger.inl"
#endif

UniformLexicon::UniformLexicon(const UniformLexiconConstructArgs& args)
	: m_Uniforms(args.uniforms)
{
}

UniformLexicon::UniformLexicon(UniformLexiconConstructArgs&& args)
	: m_Uniforms(std::move(args.uniforms))
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

// TODO pass UniformLexiconRegistry reference so that Renderer isn't always used.
void UniformLexicon::MergeLexicon(UniformLexiconHandle lexicon_handle)
{
	if (lexicon_handle != 0)
	{
		UniformLexicon const* lexicon = Renderer::UniformLexicons().Get(lexicon_handle);
		m_Uniforms.insert(lexicon->m_Uniforms.begin(), lexicon->m_Uniforms.end());
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
		auto lexicon = Renderer::UniformLexicons().Get(lexicon_handle);
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
			Renderer::Shaders().SetUniform1i(shader, name.c_str(), std::get<GLint>(uniform));
			break;
		case 1:
			Renderer::Shaders().SetUniform2iv(shader, name.c_str(), &std::get<glm::ivec2>(uniform)[0]);
			break;
		case 2:
			Renderer::Shaders().SetUniform3iv(shader, name.c_str(), &std::get<glm::ivec3>(uniform)[0]);
			break;
		case 3:
			Renderer::Shaders().SetUniform4iv(shader, name.c_str(), &std::get<glm::ivec4>(uniform)[0]);
			break;
		case 4:
			Renderer::Shaders().SetUniform1ui(shader, name.c_str(), std::get<GLuint>(uniform));
			break;
		case 5:
			Renderer::Shaders().SetUniform2uiv(shader, name.c_str(), &std::get<glm::uvec2>(uniform)[0]);
			break;
		case 6:
			Renderer::Shaders().SetUniform3uiv(shader, name.c_str(), &std::get<glm::uvec3>(uniform)[0]);
			break;
		case 7:
			Renderer::Shaders().SetUniform4uiv(shader, name.c_str(), &std::get<glm::uvec4>(uniform)[0]);
			break;
		case 8:
			Renderer::Shaders().SetUniform1f(shader, name.c_str(), std::get<GLfloat>(uniform));
			break;
		case 9:
			Renderer::Shaders().SetUniform2fv(shader, name.c_str(), &std::get<glm::vec2>(uniform)[0]);
			break;
		case 10:
			Renderer::Shaders().SetUniform3fv(shader, name.c_str(), &std::get<glm::vec3>(uniform)[0]);
			break;
		case 11:
			Renderer::Shaders().SetUniform4fv(shader, name.c_str(), &std::get<glm::vec4>(uniform)[0]);
			break;
		case 12:
			Renderer::Shaders().SetUniformMatrix2fv(shader, name.c_str(), &std::get<glm::mat2>(uniform)[0][0]);
			break;
		case 13:
			Renderer::Shaders().SetUniformMatrix3fv(shader, name.c_str(), &std::get<glm::mat3>(uniform)[0][0]);
			break;
		case 14:
			Renderer::Shaders().SetUniformMatrix4fv(shader, name.c_str(), &std::get<glm::mat4>(uniform)[0][0]);
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

UniformLexiconHandle UniformLexiconRegistry::GetHandle(UniformLexiconConstructArgs&& args)
{
	auto iter = lookup_1.find(args);
	if (iter != lookup_1.end())
		return iter->second;
	UniformLexiconHandle handle = current_handle++;
	registry.emplace(handle, std::move(args));
	lookup_1[args] = handle;
	return handle;
}

void UniformLexiconRegistry::OnApply(UniformLexiconHandle uniformLexicon, ShaderHandle shader)
{
	if (uniformLexicon == 0 || shader == 0)
		return;
	if (dynamicLexicons.find(uniformLexicon) == dynamicLexicons.end())
	{
		auto set = shaderCache.find(uniformLexicon);
		if (set != shaderCache.end())
		{
			if (set->second.find(shader) != set->second.end())
				return;
			else
				set->second.insert(shader);
		}
		else
			shaderCache.insert({ uniformLexicon, { shader } });
	}

	UniformLexicon const* lex = Get(uniformLexicon);
	if (lex)
		lex->OnApply(shader);
}

bool UniformLexiconRegistry::Shares(UniformLexiconHandle lexicon1, UniformLexiconHandle lexicon2)
{
	if (lexicon1 == lexicon2 || lexicon1 == 0 || lexicon2 == 0)
		return true;
	UniformLexicon* lex1 = nullptr;
	UniformLexicon* lex2 = nullptr;
	for (auto& [handle, lexicon] : registry)
	{
		if (handle == lexicon1)
			lex1 = &lexicon;
		else if (handle == lexicon2)
			lex2 = &lexicon;
	}
	if (lex1)
	{
		if (lex2)
		{
			return lex1->Shares(*lex2);
		}
		else
		{
#if !PULSAR_IGNORE_WARNINGS_NULL_UNIFORM_LEXICON
			Logger::LogWarning("UniformLexicon handle (" + std::to_string(lexicon2) + ") does not exist in UniformLexiconRegistry.");
#endif
			return true;
		}
	}
	else
	{
#if !PULSAR_IGNORE_WARNINGS_NULL_UNIFORM_LEXICON
		Logger::LogWarning("UniformLexicon handle (" + std::to_string(lexicon1) + ") does not exist in UniformLexiconRegistry.");
#endif
		return true;
	}
}

const Uniform* UniformLexiconRegistry::GetValue(UniformLexiconHandle lexicon, const std::string& name)
{
	UniformLexicon const* lex = Get(lexicon);
	if (lex)
	{
		return lex->GetValue(name);
	}
	else return nullptr;
}

void UniformLexiconRegistry::SetValue(UniformLexiconHandle lexicon, const std::string& name, const Uniform& value)
{
	UniformLexicon* lex = const_cast<UniformLexicon*>(Get(lexicon));
	if (lex && lex->SetValue(name, value))
		shaderCache.erase(lexicon);
}

bool UniformLexiconRegistry::DefineNewValue(UniformLexiconHandle lexicon, const std::string& name, const Uniform& value)
{
	UniformLexicon* lex = const_cast<UniformLexicon*>(Get(lexicon));
	if (lex && lex->DefineNewValue(name, value))
	{
		shaderCache.erase(lexicon);
		return true;
	}
	else return false;
}

void UniformLexiconRegistry::MarkStatic(UniformLexiconHandle lexicon)
{
	dynamicLexicons.insert(lexicon);
}

void UniformLexiconRegistry::MarkDynamic(UniformLexiconHandle lexicon)
{
	dynamicLexicons.erase(lexicon);
}
