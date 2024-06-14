#include "UniformLexiconFactory.h"

#include "Logger.h"
#include "ShaderFactory.h"

UniformLexiconHandle UniformLexiconFactory::handle_cap;
std::unordered_map<UniformLexiconHandle, UniformLexicon*> UniformLexiconFactory::factory;

void UniformLexiconFactory::Init()
{
	handle_cap = 1;
}

void UniformLexiconFactory::Terminate()
{
	for (const auto& [handle, uniformLexicon] : factory)
		delete uniformLexicon;
	factory.clear();
}

UniformLexicon* UniformLexiconFactory::Get(UniformLexiconHandle handle)
{
	auto iter = factory.find(handle);
	if (iter != factory.end())
		return iter->second;
	else
	{
		Logger::LogWarning("UniformLexicon handle (" + std::to_string(handle) + ") does not exist in UniformLexiconFactory.");
		return nullptr;
	}
}

UniformLexiconHandle UniformLexiconFactory::GetHandle(const std::map<std::string, Uniform>& uniforms)
{
	for (const auto& [handle, uniformLexicon] : factory)
	{
		if (uniformLexicon->Equivalent(uniforms))
			return handle;
	}
	UniformLexiconHandle handle = handle_cap++;
	factory.emplace(handle, new UniformLexicon(uniforms));
	return handle;
}

void UniformLexiconFactory::OnApply(const UniformLexiconHandle& uniformLexicon, const ShaderHandle& shader)
{
	// TODO optimize by caching shader/uniformLexicon pairs.
	if (uniformLexicon == 0 || shader == 0)
		return;
	UniformLexicon* lex = Get(uniformLexicon);
	if (!lex)
		return;
	for (const auto& [name, uniform] : lex->m_Uniforms)
	{
		switch (uniform.index())
		{
		case 0:
			ShaderFactory::SetUniform1i(shader, name.c_str(), std::get<GLint>(uniform));
			break;
		case 1:
			ShaderFactory::SetUniform2iv(shader, name.c_str(), &std::get<glm::ivec2>(uniform)[0]);
			break;
		case 2:
			ShaderFactory::SetUniform3iv(shader, name.c_str(), &std::get<glm::ivec3>(uniform)[0]);
			break;
		case 3:
			ShaderFactory::SetUniform4iv(shader, name.c_str(), &std::get<glm::ivec4>(uniform)[0]);
			break;
		case 4:
			ShaderFactory::SetUniform1ui(shader, name.c_str(), std::get<GLuint>(uniform));
			break;
		case 5:
			ShaderFactory::SetUniform2uiv(shader, name.c_str(), &std::get<glm::uvec2>(uniform)[0]);
			break;
		case 6:
			ShaderFactory::SetUniform3uiv(shader, name.c_str(), &std::get<glm::uvec3>(uniform)[0]);
			break;
		case 7:
			ShaderFactory::SetUniform4uiv(shader, name.c_str(), &std::get<glm::uvec4>(uniform)[0]);
			break;
		case 8:
			ShaderFactory::SetUniform1f(shader, name.c_str(), std::get<GLfloat>(uniform));
			break;
		case 9:
			ShaderFactory::SetUniform2fv(shader, name.c_str(), &std::get<glm::vec2>(uniform)[0]);
			break;
		case 10:
			ShaderFactory::SetUniform3fv(shader, name.c_str(), &std::get<glm::vec3>(uniform)[0]);
			break;
		case 11:
			ShaderFactory::SetUniform4fv(shader, name.c_str(), &std::get<glm::vec4>(uniform)[0]);
			break;
		case 12:
			ShaderFactory::SetUniformMatrix2fv(shader, name.c_str(), &std::get<glm::mat2>(uniform)[0][0]);
			break;
		case 13:
			ShaderFactory::SetUniformMatrix3fv(shader, name.c_str(), &std::get<glm::mat3>(uniform)[0][0]);
			break;
		case 14:
			ShaderFactory::SetUniformMatrix4fv(shader, name.c_str(), &std::get<glm::mat4>(uniform)[0][0]);
			break;
		}
	}
}

bool UniformLexiconFactory::Shares(const UniformLexiconHandle& lexicon1, const UniformLexiconHandle& lexicon2)
{
	if (lexicon1 == lexicon2)
		return true;
	UniformLexicon* lex1 = nullptr;
	UniformLexicon* lex2 = nullptr;
	for (const auto& [handle, lexicon] : factory)
	{
		if (handle == lexicon1)
			lex1 = lexicon;
		else if (handle == lexicon2)
			lex2 = lexicon;
	}
	if (lex1)
	{
		if (lex2)
		{
			return lex1->Shares(*lex2);
		}
		else
		{
			Logger::LogWarning("UniformLexicon handle (" + std::to_string(lexicon2) + ") does not exist in UniformLexiconFactory.");
			return false;
		}
	}
	else
	{
		Logger::LogWarning("UniformLexicon handle (" + std::to_string(lexicon1) + ") does not exist in UniformLexiconFactory.");
		return false;
	}
}
