#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <algorithm>

#include <GL/glew.h>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "Registry.inl"
#include "Handles.inl"

typedef std::variant<
	GLint, glm::ivec2, glm::ivec3, glm::ivec4,
	GLuint, glm::uvec2, glm::uvec3, glm::uvec4,
	GLfloat, glm::vec2, glm::vec3, glm::vec4,
	glm::mat2, glm::mat3, glm::mat4
> Uniform;

// TODO lexicon version and register lexicon

struct UniformLexiconConstructArgs
{
	std::unordered_map<std::string, Uniform> uniforms;

	UniformLexiconConstructArgs(const std::unordered_map<std::string, Uniform>& uniforms) : uniforms(uniforms) {}
	UniformLexiconConstructArgs(std::unordered_map<std::string, Uniform>&& uniforms) : uniforms(std::move(uniforms)) {}

	bool operator==(const UniformLexiconConstructArgs& args) const
	{
		return uniforms == args.uniforms;
	}
};

template<>
struct std::hash<UniformLexiconConstructArgs>
{
	size_t operator()(const UniformLexiconConstructArgs& args) const
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

// TODO POSSIBLY don't use class for this. Just use unordered_map, and define utility functions like Shares, MergeLexicon, GetValue, etc.
class UniformLexicon
{
	friend class UniformLexiconRegistry;
	std::unordered_map<std::string, Uniform> m_Uniforms;

public:
	UniformLexicon() = default;
	UniformLexicon(const UniformLexiconConstructArgs& args);
	UniformLexicon(UniformLexiconConstructArgs&& args);
	UniformLexicon(const UniformLexicon&);
	UniformLexicon(UniformLexicon&&) noexcept;
	UniformLexicon& operator=(const UniformLexicon&);
	UniformLexicon& operator=(UniformLexicon&&) noexcept;
	
	constexpr operator bool() const { return true; }

	void MergeLexicon(UniformLexiconHandle lexicon_handle);
	bool Shares(const UniformLexicon& lexicon);
	bool Shares(UniformLexiconHandle lexicon_handle);
	void OnApply(ShaderHandle shader) const;

private:
	Uniform const* GetValue(const std::string& name) const;
	bool SetValue(const std::string& name, const Uniform& uniform);
	bool DefineNewValue(const std::string& name, const Uniform& uniform);

	friend class CanvasLayer;
	void Clear();
};

class UniformLexiconRegistry : public Registry<UniformLexicon, UniformLexiconHandle, UniformLexiconConstructArgs>
{
	std::unordered_map<UniformLexiconHandle, std::unordered_set<ShaderHandle>> shaderCache;
	std::unordered_set<UniformLexiconHandle> dynamicLexicons;

public:
	UniformLexiconHandle GetHandle(UniformLexiconConstructArgs&& args);

	void OnApply(UniformLexiconHandle uniformLexicon, ShaderHandle shader);
	bool Shares(UniformLexiconHandle lexicon1, UniformLexiconHandle lexicon2);
	const Uniform* GetValue(UniformLexiconHandle lexicon, const std::string& name);
	void SetValue(UniformLexiconHandle lexicon, const std::string& name, const Uniform& value);
	bool DefineNewValue(UniformLexiconHandle lexicon, const std::string& name, const Uniform& value);
	void MarkStatic(UniformLexiconHandle lexicon);
	void MarkDynamic(UniformLexiconHandle lexicon);
};
