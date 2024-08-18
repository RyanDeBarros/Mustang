#include "UniformLexiconFactory.h"

#include "Macros.h"
#include "Logger.inl"
#include "ShaderFactory.h"

UniformLexiconHandle UniformLexiconFactory::handle_cap;
std::unordered_map<UniformLexiconHandle, UniformLexicon*> UniformLexiconFactory::factory;
std::unordered_map<UniformLexiconConstructArgs, UniformLexiconHandle> UniformLexiconFactory::lookupMap;
std::unordered_map<UniformLexiconHandle, std::unordered_set<ShaderHandle>> UniformLexiconFactory::shaderCache;
std::unordered_set<UniformLexiconHandle> UniformLexiconFactory::dynamicLexicons;

void UniformLexiconFactory::Init()
{
	handle_cap = 1;
}

void UniformLexiconFactory::Terminate()
{
	for (const auto& [handle, uniformLexicon] : factory)
	{
		if (uniformLexicon)
			delete uniformLexicon;
	}
	factory.clear();
	lookupMap.clear();
	shaderCache.clear();
	dynamicLexicons.clear();
}

UniformLexicon const* UniformLexiconFactory::Get(UniformLexiconHandle handle)
{
	auto iter = factory.find(handle);
	if (iter != factory.end())
		return iter->second;
	else
	{
#if !PULSAR_IGNORE_WARNINGS_NULL_UNIFORM_LEXICON
		Logger::LogWarning("UniformLexicon handle (" + std::to_string(handle) + ") does not exist in UniformLexiconFactory.");
#endif
		return nullptr;
	}
}

UniformLexiconHandle UniformLexiconFactory::GetHandle(const UniformLexiconConstructArgs& args)
{
	auto iter = lookupMap.find(args);
	if (iter != lookupMap.end())
		return iter->second;
	UniformLexiconHandle handle = handle_cap++;
	factory.emplace(handle, new UniformLexicon(args.uniforms));
	lookupMap[args] = handle;
	return handle;
}

void UniformLexiconFactory::OnApply(UniformLexiconHandle uniformLexicon, ShaderHandle shader)
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

bool UniformLexiconFactory::Shares(UniformLexiconHandle lexicon1, UniformLexiconHandle lexicon2)
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
#if !PULSAR_IGNORE_WARNINGS_NULL_UNIFORM_LEXICON
			Logger::LogWarning("UniformLexicon handle (" + std::to_string(lexicon2) + ") does not exist in UniformLexiconFactory.");
#endif
			return true;
		}
	}
	else
	{
#if !PULSAR_IGNORE_WARNINGS_NULL_UNIFORM_LEXICON
		Logger::LogWarning("UniformLexicon handle (" + std::to_string(lexicon1) + ") does not exist in UniformLexiconFactory.");
#endif
		return true;
	}
}

const Uniform* UniformLexiconFactory::GetValue(UniformLexiconHandle lexicon, const std::string& name)
{
	UniformLexicon const* lex = Get(lexicon);
	if (lex)
	{
		return lex->GetValue(name);
	}
	else return nullptr;
}

void UniformLexiconFactory::SetValue(UniformLexiconHandle lexicon, const std::string& name, const Uniform& value)
{
	UniformLexicon* lex = const_cast<UniformLexicon*>(Get(lexicon));
	if (lex && lex->SetValue(name, value))
		shaderCache.erase(lexicon);
}

bool UniformLexiconFactory::DefineNewValue(UniformLexiconHandle lexicon, const std::string& name, const Uniform& value)
{
	UniformLexicon* lex = const_cast<UniformLexicon*>(Get(lexicon));
	if (lex && lex->DefineNewValue(name, value))
	{
		shaderCache.erase(lexicon);
		return true;
	}
	else return false;
}

void UniformLexiconFactory::MarkStatic(UniformLexiconHandle lexicon)
{
	dynamicLexicons.insert(lexicon);
}

void UniformLexiconFactory::MarkDynamic(UniformLexiconHandle lexicon)
{
	dynamicLexicons.erase(lexicon);
}
