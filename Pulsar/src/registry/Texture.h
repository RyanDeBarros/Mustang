#pragma once

#include <GL/glew.h>

#include <string>

#include "Pulsar.h"
#include "TileRegistry.h"

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
	GLint lodLevel = 0;

	bool operator==(const TextureSettings& other) const
	{
		return minFilter == other.minFilter && magFilter == other.magFilter && wrapS == other.wrapS && wrapT == other.wrapT && lodLevel == other.lodLevel;
	}
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
		auto hash5 = hash<GLint>{}(settings.lodLevel);
		return hash1 ^ (hash2 << 1) ^ (hash3 << 2) ^ (hash4 << 3) ^ (hash5 << 4);
	}
};

typedef GLuint Texture_RID;

class Texture
{
	Texture_RID m_RID;
	int m_Width;
	int m_Height;
	TileHandle m_Tile;

	friend class TextureRegistry;
	bool IsValid() const { return m_RID > 0; }

public:
	Texture(const char* filepath, TextureSettings settings = {}, bool temporary_buffer = true, float svg_scale = 1.0f);
	Texture(TileHandle tile, TextureSettings settings = {});
	Texture(Tile&& tile, TextureSettings settings = {});
	Texture(const Texture& texture) = delete;
	Texture(Texture&& texture) noexcept;
	Texture& operator=(Texture&& texture) noexcept;
	~Texture();

	void Bind(TextureSlot slot = 0) const;
	void Unbind(TextureSlot slot = 0) const;

	void SetSettings(const TextureSettings& settings) const;

	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	TileHandle GetTileHandle() const { return m_Tile; }

	static const TextureSettings linear_settings;
	static const TextureSettings nearest_settings;

private:
	void TexImage(Tile const* tile, const TextureSettings& settings, const std::string& err_msg);
};
