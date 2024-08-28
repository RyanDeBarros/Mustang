#pragma once

#include <unordered_map>

#include "Texture.h"
#include "Handles.inl"

struct TextureConstructArgs_filepath
{
	std::string filepath;
	TextureSettings settings = {};
	bool temporaryBuffer = false;
	TextureVersion version = 0;
	float svg_scale = 1.0f;

	inline bool operator==(const TextureConstructArgs_filepath& args) const
	{
		return filepath == args.filepath && settings == args.settings && temporaryBuffer == args.temporaryBuffer && version == args.version && svg_scale == args.svg_scale;
	}
};

template<>
struct std::hash<TextureConstructArgs_filepath>
{
	inline size_t operator()(const TextureConstructArgs_filepath& args) const
	{
		auto hash1 = hash<std::string>{}(args.filepath);
		auto hash2 = hash<TextureSettings>{}(args.settings);
		auto hash3 = hash<bool>{}(args.temporaryBuffer);
		auto hash4 = hash<TextureVersion>{}(args.version);
		auto hash5 = hash<float>{}(args.svg_scale);
		return hash1 ^ (hash2 << 1) ^ (hash3 << 2) ^ (hash4 << 3) ^ (hash5 << 4);
	}
};

struct TextureConstructArgs_tile
{
	TileHandle tile;
	TextureVersion version = 0;
	TextureSettings settings = {};

	inline bool operator==(const TextureConstructArgs_tile& args) const
	{
		return tile == args.tile && settings == args.settings && version == args.version;
	}
};

template<>
struct std::hash<TextureConstructArgs_tile>
{
	inline size_t operator()(const TextureConstructArgs_tile& args) const
	{
		auto hash1 = hash<TileHandle>{}(args.tile);
		auto hash2 = hash<TextureVersion>{}(args.version);
		auto hash3 = hash<TextureSettings>{}(args.settings);
		return hash1 ^ (hash2 << 1) ^ (hash3 < 2);
	}
};

class TextureRegistry
{
	static TextureHandle handle_cap;
	static std::unordered_map<TextureHandle, Texture> registry;
	static std::unordered_map<TextureConstructArgs_filepath, TextureHandle> lookupMap_filepath;
	static std::unordered_map<TextureConstructArgs_tile, TextureHandle> lookupMap_tile;

	TextureRegistry() = delete;
	TextureRegistry(const TextureRegistry&) = delete;
	TextureRegistry(TextureRegistry&&) = delete;
	~TextureRegistry() = delete;

	friend class Renderer;
	static void Init();
	static void Terminate();

public:
	static Texture const* Get(TextureHandle handle);
	static bool DestroyTexture(TextureHandle handle);
	static TextureHandle GetHandle(const TextureConstructArgs_filepath& args);
	static TextureHandle GetHandle(const TextureConstructArgs_tile& args);
	static TextureHandle RegisterTexture(Texture&& texture);
	static void Bind(TextureHandle handle, TextureSlot slot);
	static void Unbind(TextureSlot slot);
	
	inline static int GetWidth(TextureHandle handle) { Texture const* texture = Get(handle); return texture ? texture->GetWidth() : 0; }
	inline static int GetHeight(TextureHandle handle) { Texture const* texture = Get(handle); return texture ? texture->GetHeight() : 0; }
	inline static TileHandle GetTileHandle(TextureHandle handle) { Texture const* texture = Get(handle); return texture ? texture->GetTileHandle() : 0; }
	static void SetSettings(TextureHandle handle, const TextureSettings& settings);
};
