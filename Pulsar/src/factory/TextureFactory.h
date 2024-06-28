#pragma once

#include <unordered_map>

#include "Typedefs.h"
#include "Texture.h"

class TextureFactory
{
	static TextureHandle handle_cap;
	static std::unordered_map<TextureHandle, Texture*> factory;
	static Texture* Get(TextureHandle);
	static TextureHandle CreateHandle(const char* filepath, const TextureSettings& settings, const bool& temporary_buffer);
	static TextureHandle CreateHandle(const TileHandle& tile, const TextureSettings& settings);

	TextureFactory() = delete;
	~TextureFactory() = delete;

	friend class Renderer;
	static void Init();
	static void Terminate();

public:
	static TextureHandle GetHandle(const char* filepath, const TextureSettings& settings = TextureSettings(), const bool& new_texture = false, const bool& temporary_buffer = false);
	static TextureHandle GetHandle(const TileHandle& tile, const TextureSettings& settings = TextureSettings(), const bool& new_texture = false);
	static void Bind(const TextureHandle& handle, const TextureSlot& slot);
	static void Unbind(const TextureSlot& slot);
	
	inline static int GetWidth(const TextureHandle& handle) { Texture* texture = Get(handle); return texture ? texture->GetWidth() : 0; }
	inline static int GetHeight(const TextureHandle& handle) { Texture* texture = Get(handle); return texture ? texture->GetHeight() : 0; }
	inline static TileHandle GetTileHandle(const TextureHandle& handle) { Texture* texture = Get(handle); return texture ? texture->GetTileHandle() : 0; }
	static void SetSettings(const TextureHandle& handle, const TextureSettings& settings);
};
