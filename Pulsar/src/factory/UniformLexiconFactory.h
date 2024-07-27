#pragma once

#include <map>
#include <unordered_map>
#include <unordered_set>

#include "Typedefs.h"
#include "UniformLexicon.h"

class UniformLexiconFactory
{
	static UniformLexiconHandle handle_cap;
	static std::unordered_map<UniformLexiconHandle, UniformLexicon*> factory;
	static UniformLexicon* Get(UniformLexiconHandle);
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
	static UniformLexiconHandle GetHandle(const std::map<std::string, Uniform>& uniforms);
	static void OnApply(UniformLexiconHandle uniformLexicon, ShaderHandle shader);
	static bool Shares(UniformLexiconHandle lexicon1, UniformLexiconHandle lexicon2);
	inline static const UniformLexicon* GetConstRef(UniformLexiconHandle lexicon) { return Get(lexicon); }
	static const Uniform* GetValue(UniformLexiconHandle lexicon, const std::string& name);
	static void SetValue(UniformLexiconHandle lexicon, const std::string& name, const Uniform& value);
	static bool DefineNewValue(UniformLexiconHandle lexicon, const std::string& name, const Uniform& value);
	static void MarkStatic(UniformLexiconHandle lexicon);
	static void MarkDynamic(UniformLexiconHandle lexicon);
};
