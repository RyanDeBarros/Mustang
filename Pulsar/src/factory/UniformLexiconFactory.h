#pragma once

#include <map>
#include <unordered_map>

#include "Typedefs.h"
#include "UniformLexicon.h"

class UniformLexiconFactory
{
	static UniformLexiconHandle handle_cap;
	static std::unordered_map<UniformLexiconHandle, UniformLexicon*> factory;
	static UniformLexicon* Get(UniformLexiconHandle);

	UniformLexiconFactory() = delete;
	~UniformLexiconFactory() = delete;

	friend class Renderer;
	static void Init();
	static void Terminate();

public:
	static UniformLexiconHandle GetHandle(const std::map<std::string, Uniform>& uniforms);
	static void OnApply(const UniformLexiconHandle& uniformLexicon, const ShaderHandle& shader);
	static bool Shares(const UniformLexiconHandle& lexicon1, const UniformLexiconHandle& lexicon2);
};
