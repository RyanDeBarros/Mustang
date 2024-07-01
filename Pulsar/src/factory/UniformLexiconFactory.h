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
	~UniformLexiconFactory() = delete;

	friend class Renderer;
	static void Init();
	static void Terminate();

public:
	static UniformLexiconHandle GetHandle(const std::map<std::string, Uniform>& uniforms);
	static void OnApply(const UniformLexiconHandle& uniformLexicon, const ShaderHandle& shader);
	static bool Shares(const UniformLexiconHandle& lexicon1, const UniformLexiconHandle& lexicon2);
	inline static const UniformLexicon* GetConstRef(const UniformLexiconHandle& lexicon) { return Get(lexicon); }
	static const Uniform* GetValue(const UniformLexiconHandle& lexicon, const std::string& name);
	static void SetValue(const UniformLexiconHandle& lexicon, const std::string& name, const Uniform& value);
	static bool DefineNewValue(const UniformLexiconHandle& lexicon, const std::string& name, const Uniform& value);
	static void MarkStatic(const UniformLexiconHandle& lexicon);
	static void MarkDynamic(const UniformLexiconHandle& lexicon);
};
