#pragma once

#include <unordered_map>

#include "Typedefs.h"
#include "Material.h"

class MaterialFactory
{
	static MaterialHandle handle_cap;
	static std::unordered_map<TextureHandle, Material*> factory;
	static Material* Get(MaterialHandle);

	MaterialFactory() = delete;
	~MaterialFactory() = delete;

	friend class Renderer;
	static void Init();
	static void Terminate();

public:
	static MaterialHandle GetHandle(const std::vector<MaterialUniform>& uniform_vector);
	static void Apply(const MaterialHandle& material, const ShaderHandle& shader);
};
