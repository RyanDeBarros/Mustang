#include "MaterialFactory.h"

#include "Logger.h"

MaterialHandle MaterialFactory::handle_cap;
std::unordered_map<TextureHandle, Material*> MaterialFactory::factory;

void MaterialFactory::Init()
{
	handle_cap = 1;
}

void MaterialFactory::Terminate()
{
	for (const auto& [handle, material] : factory)
		delete material;
	factory.clear();
}

Material* MaterialFactory::Get(MaterialHandle handle)
{
	auto iter = factory.find(handle);
	if (iter != factory.end())
		return iter->second;
	else
	{
		Logger::LogWarning("Material handle (" + std::to_string(handle) + ") does not exist in MaterialFactory.");
		return nullptr;
	}
}

MaterialHandle MaterialFactory::GetHandle(const std::vector<MaterialUniform>& uniform_vector)
{
	for (const auto& [handle, material] : factory)
	{
		if (material->equivalent(uniform_vector))
			return handle;
	}
	MaterialHandle handle = handle_cap++;
	factory.emplace(handle, new Material(uniform_vector));
	return handle;
}

void MaterialFactory::Apply(const MaterialHandle& handle)
{
	// TODO apply material to currently bound shader
}
