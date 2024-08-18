#pragma once

#include <map>
#include <unordered_map>
#include <unordered_set>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include "Typedefs.h"
#include "UniformLexicon.h"

struct UniformLexiconConstructArgs
{
	const std::unordered_map<std::string, Uniform>& uniforms;

	inline bool operator==(const UniformLexiconConstructArgs& args) const
	{
		return uniforms == args.uniforms;
	}
};

template<>
struct std::hash<UniformLexiconConstructArgs>
{
	inline size_t operator()(const UniformLexiconConstructArgs& args) const
	{
		size_t hasher = 0;
		size_t i = 0;
		for (auto iter = args.uniforms.begin(); iter != args.uniforms.end(); iter++)
		{
			auto h1 = hash<std::string>{}(iter->first);
			auto h2 = hash<Uniform>{}(iter->second);
			hasher ^= (h1 << i) ^ (h2 << i);
			i++;
		}
		return hasher;
	}
};

class UniformLexiconFactory
{
	static UniformLexiconHandle handle_cap;
	static std::unordered_map<UniformLexiconHandle, UniformLexicon*> factory;
	static std::unordered_map<UniformLexiconConstructArgs, UniformLexiconHandle> lookupMap;
	static std::unordered_map<UniformLexiconHandle, std::unordered_set<ShaderHandle>> shaderCache;
	static std::unordered_set<UniformLexiconHandle> dynamicLexicons;

	UniformLexiconFactory() = delete;
	UniformLexiconFactory(const UniformLexiconFactory&) = delete;
	UniformLexiconFactory(UniformLexiconFactory&&) = delete;
	~UniformLexiconFactory() = delete;

	friend class Renderer;
	static void Init();
	static void Terminate();

public:
	static UniformLexiconHandle GetHandle(const UniformLexiconConstructArgs& args);
	static UniformLexicon const* Get(UniformLexiconHandle);
	static void OnApply(UniformLexiconHandle uniformLexicon, ShaderHandle shader);
	static bool Shares(UniformLexiconHandle lexicon1, UniformLexiconHandle lexicon2);
	static const Uniform* GetValue(UniformLexiconHandle lexicon, const std::string& name);
	static void SetValue(UniformLexiconHandle lexicon, const std::string& name, const Uniform& value);
	static bool DefineNewValue(UniformLexiconHandle lexicon, const std::string& name, const Uniform& value);
	static void MarkStatic(UniformLexiconHandle lexicon);
	static void MarkDynamic(UniformLexiconHandle lexicon);
};
