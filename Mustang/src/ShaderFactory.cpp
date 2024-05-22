#include "ShaderFactory.h"
#include "Shader.h"
#include "Logger.h"

#include <unordered_map>


struct ShaderElement
{
	Shader* shader;
	char* vertexFilepath;
	char* fragmentFilepath;
	ShaderElement()
		: shader(nullptr), vertexFilepath(nullptr), fragmentFilepath(nullptr)
	{}
	ShaderElement(ShaderElement&& move) noexcept
		: shader(move.shader), vertexFilepath(move.vertexFilepath), fragmentFilepath(move.fragmentFilepath)
	{
		move.shader = nullptr;
		move.vertexFilepath = nullptr;
		move.fragmentFilepath = nullptr;
	}
	ShaderElement(const char* vertex_filepath, const char* fragment_filepath)
		: shader(new Shader(vertex_filepath, fragment_filepath))
	{
		size_t length = strlen(vertex_filepath);
		vertexFilepath = new char[length + 1];
		strcpy_s(vertexFilepath, length + 1, vertex_filepath);
		vertexFilepath[length] = '\0';
		length = strlen(fragment_filepath);
		fragmentFilepath = new char[length + 1];
		strcpy_s(fragmentFilepath, length + 1, fragment_filepath);
		fragmentFilepath[length] = '\0';
	}
	~ShaderElement()
	{
		if (shader)
			delete shader;
		if (vertexFilepath)
			delete[] vertexFilepath;
		if (fragmentFilepath)
			delete[] fragmentFilepath;
	}
};

static ShaderHandle handle_cap = 1;
std::unordered_map<ShaderHandle, ShaderElement> factory;

ShaderHandle ShaderFactory::GetHandle(const char* vertex_shader, const char* fragment_shader)
{
	for (const auto& [handle, element] : factory)
	{
		if (strcmp(vertex_shader, element.vertexFilepath) == 0 && strcmp(fragment_shader, element.fragmentFilepath) == 0)
			return handle;
	}
	ShaderHandle handle = handle_cap++;
	factory.emplace(handle, ShaderElement(vertex_shader, fragment_shader));
	return handle;
}

void ShaderFactory::Bind(ShaderHandle handle)
{
	auto iter = factory.find(handle);
	if (iter != factory.end())
		iter->second.shader->Bind();
	else
		Logger::LogWarning("Failed to bind shader. Handle (" + std::to_string(handle) + ") does not exist in ShaderFactory.");
}

void ShaderFactory::Unbind(ShaderHandle handle)
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
