#include "ShaderFactory.h"
#include "Shader.h"
#include "Logger.h"

#include <unordered_map>


struct ShaderElement
{
	Shader* shader;
	const char* vertexFilepath;
	const char* fragmentFilepath;
	ShaderElement()
		: shader(nullptr), vertexFilepath(nullptr), fragmentFilepath(nullptr)
	{}
	ShaderElement(ShaderElement&& move) noexcept
		: shader(move.shader), vertexFilepath(move.vertexFilepath), fragmentFilepath(move.fragmentFilepath)
	{
		move.shader = nullptr;
	}
	ShaderElement(const char* vertex_filepath, const char* fragment_filepath)
		: shader(new Shader(vertex_filepath, fragment_filepath)), vertexFilepath(vertex_filepath), fragmentFilepath(fragment_filepath)
	{}
	~ShaderElement()
	{
		if (shader)
			delete shader;
	}
};

static unsigned int handle_cap = 1;
std::unordered_map<unsigned int, ShaderElement> factory;

GLuint ShaderFactory::GetHandle(const char* vertex_shader, const char* fragment_shader)
{
	for (const auto& [handle, element] : factory)
	{
		if (strcmp(vertex_shader, element.vertexFilepath) == 0 && strcmp(fragment_shader, element.fragmentFilepath) == 0)
			return handle;
	}
	unsigned int handle = handle_cap++;
	factory.emplace(handle, ShaderElement(vertex_shader, fragment_shader));
	return handle;
}

void ShaderFactory::Bind(unsigned int handle)
{
	auto iter = factory.find(handle);
	if (iter != factory.end())
		iter->second.shader->Bind();
	else
		Logger::LogWarning("Failed to bind shader. Handle (" + std::to_string(handle) + ") does not exist in ShaderFactory.");
}

void ShaderFactory::Unbind(unsigned int handle)
{
	auto iter = factory.find(handle);
	if (iter != factory.end())
		iter->second.shader->Unbind();
	else
		Logger::LogWarning("Failed to unbind shader. Handle (" + std::to_string(handle) + ") does not exist in ShaderFactory.");
}

void ShaderFactory::Terminate()
{
	factory.clear();
}
