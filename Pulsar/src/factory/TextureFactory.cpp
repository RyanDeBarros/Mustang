#include "TextureFactory.h"

#include "Logger.h"
#include "Macros.h"

TextureHandle TextureFactory::handle_cap;
std::unordered_map<TextureHandle, Texture*> TextureFactory::factory;

Texture* TextureFactory::Get(TextureHandle handle)
{
	auto iter = factory.find(handle);
	if (iter != factory.end())
		return iter->second;
	else
	{
		Logger::LogWarning("Texture handle (" + std::to_string(handle) + ") does not exist in TextureFactory.");
		return nullptr;
	}
}

void TextureFactory::Init()
{
	handle_cap = 1;
}

void TextureFactory::Terminate()
{
	for (const auto& [handle, texture] : factory)
		delete texture;
	factory.clear();
}

TextureHandle TextureFactory::GetHandle(const char* filepath, TextureSettings settings)
{
	for (const auto& [handle, texture] : factory)
	{
		if (texture->Equivalent(filepath, settings))
			return handle;
	}
	Texture texture(filepath, settings);
	if (texture.IsValid())
	{
		TextureHandle handle = handle_cap++;
		factory.emplace(handle, new Texture(std::move(texture)));
		return handle;
	}
	else return 0;
}

void TextureFactory::Bind(const TextureHandle& handle, const TextureSlot& slot)
{
	Texture* texture = Get(handle);
	if (texture)
		texture->Bind(slot);
	else
		Logger::LogWarning("Failed to bind texture at handle (" + std::to_string(handle) + ") to slot (" + std::to_string(slot) + ").");
}

void TextureFactory::Unbind(const TextureSlot& slot)
{
	TRY(glActiveTexture(GL_TEXTURE0 + slot));
	TRY(glBindTexture(GL_TEXTURE_2D, 0));
}
