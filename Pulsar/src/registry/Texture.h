#pragma once

#include <GL/glew.h>

#include <string>

#include "Pulsar.h"
#include "Tile.h"
#include "Registry.inl"

enum class MinFilter : GLint
{
	Nearest = GL_NEAREST,
	Linear = GL_LINEAR,
	NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
	LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
	NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
	LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
};

constexpr MinFilter MinFilterLookup[] = {
	MinFilter::Nearest,
	MinFilter::Linear,
	MinFilter::NearestMipmapNearest,
	MinFilter::LinearMipmapNearest,
	MinFilter::NearestMipmapLinear,
	MinFilter::LinearMipmapLinear
};

constexpr unsigned int MinFilterLookupLength = 6;

enum class MagFilter : GLint
{
	Nearest = GL_NEAREST,
	Linear = GL_LINEAR
};

constexpr MagFilter MagFilterLookup[] = {
	MagFilter::Nearest,
	MagFilter::Linear
};

constexpr unsigned int MagFilterLookupLength = 2;

enum class TextureWrap : GLint
{
	ClampToEdge = GL_CLAMP_TO_EDGE,
	ClampToBorder = GL_CLAMP_TO_BORDER,
	MirroredRepeat = GL_MIRRORED_REPEAT,
	Repeat = GL_REPEAT,
	MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
};

constexpr TextureWrap TextureWrapLookup[] = {
	TextureWrap::ClampToEdge,
	TextureWrap::ClampToBorder,
	TextureWrap::MirroredRepeat,
	TextureWrap::Repeat,
	TextureWrap::MirrorClampToEdge
};

constexpr unsigned int TextureWrapLookupLength = 5;

struct TextureSettings
{
	MinFilter minFilter = MinFilter::Linear;
	MagFilter magFilter = MagFilter::Linear;
	TextureWrap wrapS = TextureWrap::ClampToEdge;
	TextureWrap wrapT = TextureWrap::ClampToEdge;

	bool operator==(const TextureSettings&) const = default;
};

template<>
struct std::hash<TextureSettings>
{
	size_t operator()(const TextureSettings& settings) const
	{
		auto hash1 = hash<MinFilter>{}(settings.minFilter);
		auto hash2 = hash<MagFilter>{}(settings.magFilter);
		auto hash3 = hash<TextureWrap>{}(settings.wrapS);
		auto hash4 = hash<TextureWrap>{}(settings.wrapT);
		return hash1 ^ (hash2 << 1) ^ (hash3 << 2) ^ (hash4 << 3);
	}
};

struct TextureConstructArgs_filepath
{
	std::string filepath;
	TextureSettings settings = {};
	bool temporary_buffer = false;
	TextureVersion version = 0;
	float svg_scale = 1.0f;

	TextureConstructArgs_filepath(const std::string& filepath, const TextureSettings& settings = {},
		bool temporary_buffer = false, TextureVersion version = 0, float svg_scale = 1.0f)
		: filepath(filepath), settings(settings), temporary_buffer(temporary_buffer), version(version), svg_scale(svg_scale) {}
	TextureConstructArgs_filepath(std::string&& filepath, const TextureSettings& settings = {},
		bool temporary_buffer = false, TextureVersion version = 0, float svg_scale = 1.0f)
		: filepath(std::move(filepath)), settings(settings), temporary_buffer(temporary_buffer), version(version), svg_scale(svg_scale) {}

	bool operator==(const TextureConstructArgs_filepath&) const = default;
};

template<>
struct std::hash<TextureConstructArgs_filepath>
{
	size_t operator()(const TextureConstructArgs_filepath& args) const
	{
		auto hash1 = hash<std::string>{}(args.filepath);
		auto hash2 = hash<TextureSettings>{}(args.settings);
		return hash1 ^ (hash2 << 1);
	}
};

struct TextureConstructArgs_tile
{
	TileHandle tile;
	TextureVersion version = 0;
	TextureSettings settings = {};

	TextureConstructArgs_tile(TileHandle tile, TextureVersion version = 0, const TextureSettings& settings = {})
		: tile(tile), version(version), settings(settings) {}

	bool operator==(const TextureConstructArgs_tile&) const = default;
};

template<>
struct std::hash<TextureConstructArgs_tile>
{
	size_t operator()(const TextureConstructArgs_tile& args) const
	{
		auto hash1 = hash<TileHandle>{}(args.tile);
		auto hash2 = hash<TextureVersion>{}(args.version);
		auto hash3 = hash<TextureSettings>{}(args.settings);
		return hash1 ^ (hash2 << 1) ^ (hash3 < 2);
	}
};

typedef GLuint Texture_RID;

class Texture
{
	Texture_RID m_RID;
	int m_Width;
	int m_Height;
	TileHandle m_Tile;

public:
	//Texture(const char* filepath, TextureSettings settings = {}, bool temporary_buffer = true, float svg_scale = 1.0f);
	Texture(const TextureConstructArgs_filepath& args);
	//Texture(TileHandle tile, TextureSettings settings = {});
	Texture(const TextureConstructArgs_tile& args);
	Texture(Tile&& tile, TextureSettings settings = {});
	Texture(const Texture& texture) = delete;
	Texture(Texture&& texture) noexcept;
	Texture& operator=(Texture&& texture) noexcept;
	~Texture();
	
	operator bool() const { return m_RID > 0; }

	void Bind(TextureSlot slot = 0) const;
	void Unbind(TextureSlot slot = 0) const;

	void SetSettings(const TextureSettings& settings) const;
	TextureSettings GetSettings() const;
	void ReTexImage(Tile const* tile, GLint lod_level = 0);
	void ReTexImage(GLint lod_level = 0);

	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	TileHandle GetTileHandle() const { return m_Tile; }

	static const TextureSettings linear_settings;
	static const TextureSettings nearest_settings;

private:
	void TexImage(Tile const* tile, const std::string& err_msg, GLint lod_level = 0);
};

class TextureRegistry : public Registry<Texture, TextureHandle, TextureConstructArgs_filepath, TextureConstructArgs_tile>
{
public:
	void Bind(TextureHandle handle, TextureSlot slot);
	void Unbind(TextureSlot slot);

	int GetWidth(TextureHandle handle) { Texture const* texture = Get(handle); return texture ? texture->GetWidth() : 0; }
	int GetHeight(TextureHandle handle) { Texture const* texture = Get(handle); return texture ? texture->GetHeight() : 0; }
	TileHandle GetTileHandle(TextureHandle handle) { Texture const* texture = Get(handle); return texture ? texture->GetTileHandle() : 0; }
	void SetSettings(TextureHandle handle, const TextureSettings& settings);
};
