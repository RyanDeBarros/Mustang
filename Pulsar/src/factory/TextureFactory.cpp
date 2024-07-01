#include "TextureFactory.h"

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include "Macros.h"
#include "Logger.h"

TextureHandle TextureFactory::handle_cap;
std::unordered_map<TextureHandle, Texture*> TextureFactory::factory;

Texture* TextureFactory::Get(TextureHandle handle)
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
}

TextureHandle TextureFactory::CreateHandle(const char* filepath, const TextureSettings& settings, const bool& temporary_buffer)
{
	Texture texture(filepath, settings, temporary_buffer);
	if (texture.IsValid())
	{
		TextureHandle handle = handle_cap++;
		factory.emplace(handle, new Texture(std::move(texture)));
		return handle;
	}
	else return 0;
}

TextureHandle TextureFactory::CreateHandle(const TileHandle& tile, const TextureSettings& settings)
{
	Texture texture(tile, settings);
	if (texture.IsValid())
	{
		TextureHandle handle = handle_cap++;
		factory.emplace(handle, new Texture(std::move(texture)));
		return handle;
	}
	else return 0;
}

TextureHandle TextureFactory::GetHandle(const char* filepath, const TextureSettings& settings, const bool& new_texture, const bool& temporary_buffer)
{
	if (new_texture)
		return CreateHandle(filepath, settings, temporary_buffer);
	for (const auto& [handle, texture] : factory)
	{
		if (texture->Equivalent(filepath, settings))
			return handle;
	}
	return CreateHandle(filepath, settings, temporary_buffer);
}

TextureHandle TextureFactory::GetHandle(const TileHandle& tile, const TextureSettings& settings, const bool& new_texture)
{
	if (new_texture)
		return CreateHandle(tile, settings);
	for (const auto& [handle, texture] : factory)
	{
		if (texture->Equivalent(tile, settings))
			return handle;
	}
	return CreateHandle(tile, settings);
}

void TextureFactory::Bind(const TextureHandle& handle, const TextureSlot& slot)
{
	Texture* texture = Get(handle);
	if (texture)
		texture->Bind(slot);
#if !PULSAR_IGNORE_WARNINGS_NULL_TEXTURE
	else
		Logger::LogWarning("Failed to bind texture at handle (" + std::to_string(handle) + ") to slot (" + std::to_string(slot) + ").");
#endif
}

void TextureFactory::Unbind(const TextureSlot& slot)
{
	TRY(glActiveTexture(GL_TEXTURE0 + slot));
	TRY(glBindTexture(GL_TEXTURE_2D, 0));
}

void TextureFactory::SetSettings(const TextureHandle& handle, const TextureSettings& settings)
{
	Texture* texture = Get(handle);
	if (texture)
		texture->SetSettings(settings);
#if !PULSAR_IGNORE_WARNINGS_NULL_TEXTURE
	else
		Logger::LogWarning("Failed to set settings at texture handle (" + std::to_string(handle) + ").");
#endif
}
