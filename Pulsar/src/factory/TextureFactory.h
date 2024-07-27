#pragma once

#include <unordered_map>

#include "Typedefs.h"
#include "Texture.h"

class TextureFactory
{
	static TextureHandle handle_cap;
	static std::unordered_map<TextureHandle, Texture*> factory;
	static Texture* Get(TextureHandle);
	static TextureHandle CreateHandle(const char* filepath, const TextureSettings& settings, bool temporary_buffer);
	static TextureHandle CreateHandle(TileHandle tile, const TextureSettings& settings);

	TextureFactory() = delete;
	TextureFactory(const TextureFactory&) = delete;
	TextureFactory(TextureFactory&&) = delete;
	~TextureFactory() = delete;

	friend class Renderer;
	static void Init();
	static void Terminate();

public:
	static TextureHandle GetHandle(const char* filepath, const TextureSettings& settings = {}, bool new_texture = false, bool temporary_buffer = false);
	static TextureHandle GetHandle(TileHandle tile, const TextureSettings& settings = {}, bool new_texture = false);
	static void Bind(TextureHandle handle, TextureSlot slot);
	static void Unbind(TextureSlot slot);
	
	inline static int GetWidth(TextureHandle handle) { Texture* texture = Get(handle); return texture ? texture->GetWidth() : 0; }
	inline static int GetHeight(TextureHandle handle) { Texture* texture = Get(handle); return texture ? texture->GetHeight() : 0; }
	inline static TileHandle GetTileHandle(TextureHandle handle) { Texture* texture = Get(handle); return texture ? texture->GetTileHandle() : 0; }
	static void SetSettings(TextureHandle handle, const TextureSettings& settings);
};
