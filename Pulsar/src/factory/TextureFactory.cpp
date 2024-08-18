#include "TextureFactory.h"

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include "Macros.h"
#include "Logger.inl"

TextureHandle TextureFactory::handle_cap;
std::unordered_map<TextureHandle, Texture*> TextureFactory::factory;
std::unordered_map<TextureConstructArgs_filepath, TextureHandle> TextureFactory::lookupMap_filepath;
std::unordered_map<TextureConstructArgs_tile, TextureHandle> TextureFactory::lookupMap_tile;

Texture const* TextureFactory::Get(TextureHandle handle)
{
	auto iter = factory.find(handle);
	if (iter != factory.end())
		return iter->second;
	else
	{
#if !PULSAR_IGNORE_WARNINGS_NULL_TEXTURE
		Logger::LogWarning("Texture handle (" + std::to_string(handle) + ") does not exist in TextureFactory.");
#endif
		return nullptr;
	}
}

void TextureFactory::Init()
{
	handle_cap = 1;
	stbi_set_flip_vertically_on_load(1);
	stbi_flip_vertically_on_write(1);
}

void TextureFactory::Terminate()
{
	for (const auto& [handle, texture] : factory)
	{
		if (texture)
			delete texture;
	}
	factory.clear();
	lookupMap_filepath.clear();
	lookupMap_tile.clear();
}

TextureHandle TextureFactory::GetHandle(const TextureConstructArgs_filepath& args)
{
	auto iter = lookupMap_filepath.find(args);
	if (iter != lookupMap_filepath.end())
		return iter->second;
	Texture texture(args.filepath.c_str(), args.settings, args.temporaryBuffer);
	if (texture.IsValid())
	{
		TextureHandle handle = handle_cap++;
		factory.emplace(handle, new Texture(std::move(texture)));
		lookupMap_filepath[args] = handle;
		return handle;
	}
	else return 0;
}

TextureHandle TextureFactory::GetHandle(const TextureConstructArgs_tile& args)
{
	auto iter = lookupMap_tile.find(args);
	if (iter != lookupMap_tile.end())
		return iter->second;
	Texture texture(args.tile, args.settings);
	if (texture.IsValid())
	{
		TextureHandle handle = handle_cap++;
		factory.emplace(handle, new Texture(std::move(texture)));
		lookupMap_tile[args] = handle;
		return handle;
	}
	else return 0;
}

void TextureFactory::Bind(TextureHandle handle, TextureSlot slot)
{
	Texture const* texture = Get(handle);
	if (texture)
		texture->Bind(slot);
#if !PULSAR_IGNORE_WARNINGS_NULL_TEXTURE
	else
		Logger::LogWarning("Failed to bind texture at handle (" + std::to_string(handle) + ") to slot (" + std::to_string(slot) + ").");
#endif
}

void TextureFactory::Unbind(TextureSlot slot)
{
	TRY(glActiveTexture(GL_TEXTURE0 + slot));
	TRY(glBindTexture(GL_TEXTURE_2D, 0));
}

void TextureFactory::SetSettings(TextureHandle handle, const TextureSettings& settings)
{
	Texture const* texture = Get(handle);
	if (texture)
		texture->SetSettings(settings);
#if !PULSAR_IGNORE_WARNINGS_NULL_TEXTURE
	else
		Logger::LogWarning("Failed to set settings at texture handle (" + std::to_string(handle) + ").");
#endif
}
