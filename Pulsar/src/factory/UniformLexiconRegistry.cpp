#include "UniformLexiconRegistry.h"

#include "Macros.h"
#include "Logger.inl"
#include "ShaderRegistry.h"

UniformLexiconHandle UniformLexiconRegistry::handle_cap;
std::unordered_map<UniformLexiconHandle, UniformLexicon> UniformLexiconRegistry::registry;
std::unordered_map<UniformLexiconConstructArgs, UniformLexiconHandle> UniformLexiconRegistry::lookupMap;
std::unordered_map<UniformLexiconHandle, std::unordered_set<ShaderHandle>> UniformLexiconRegistry::shaderCache;
std::unordered_set<UniformLexiconHandle> UniformLexiconRegistry::dynamicLexicons;

void UniformLexiconRegistry::Init()
{
	handle_cap = 1;
}

void UniformLexiconRegistry::Terminate()
{
	registry.clear();
	lookupMap.clear();
	shaderCache.clear();
	dynamicLexicons.clear();
}

UniformLexicon const* UniformLexiconRegistry::Get(UniformLexiconHandle handle)
{
	auto iter = registry.find(handle);
	if (iter != registry.end())
		return &iter->second;
	else
	{
#if !PULSAR_IGNORE_WARNINGS_NULL_UNIFORM_LEXICON
		Logger::LogWarning("UniformLexicon handle (" + std::to_string(handle) + ") does not exist in UniformLexiconRegistry.");
#endif
		return nullptr;
	}
}

bool UniformLexiconRegistry::DestroyUniformLexicon(UniformLexiconHandle handle)
{
	auto iter = registry.find(handle);
	if (iter == registry.end())
		return false;
	registry.erase(iter);
	return true;
}

UniformLexiconHandle UniformLexiconRegistry::GetHandle(const UniformLexiconConstructArgs& args)
{
	auto iter = lookupMap.find(args);
	if (iter != lookupMap.end())
		return iter->second;
	UniformLexiconHandle handle = handle_cap++;
	registry.emplace(handle, args.uniforms);
	lookupMap[args] = handle;
	return handle;
}

void UniformLexiconRegistry::OnApply(UniformLexiconHandle uniformLexicon, ShaderHandle shader)
{
	if (uniformLexicon == 0 || shader == 0)
		return;
	if (dynamicLexicons.find(uniformLexicon) == dynamicLexicons.end())
	{
		std::unordered_map<UniformLexiconHandle, std::unordered_set<ShaderHandle>>::iterator set;
		if ((set = shaderCache.find(uniformLexicon)) != shaderCache.end())
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
	if (!lex)
		return;
	for (const auto& [name, uniform] : lex->m_Uniforms)
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

bool UniformLexiconRegistry::Shares(UniformLexiconHandle lexicon1, UniformLexiconHandle lexicon2)
{
	if (lexicon1 == lexicon2)
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
