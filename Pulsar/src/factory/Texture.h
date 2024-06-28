#pragma once

#include <GL/glew.h>

#include <string>

#include "Typedefs.h"
#include "TileFactory.h"

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
	MinFilter min_filter = MinFilter::Linear;
	MagFilter mag_filter = MagFilter::Linear;
	TextureWrap wrap_s = TextureWrap::ClampToEdge;
	TextureWrap wrap_t = TextureWrap::ClampToEdge;
	GLint lod_level = 0;

	bool operator==(const TextureSettings& other) const
	{
		return min_filter == other.min_filter && mag_filter == other.mag_filter && wrap_s == other.wrap_s && wrap_t == other.wrap_t && lod_level == other.lod_level;
	}
};

class Texture
{
	RID m_RID;
	TileHandle m_Tile;
	TextureSettings m_Settings;
	// TODO AtlasHandle instead
	//const class Atlas* const m_Atlas;

	friend class TextureFactory;
	inline bool Equivalent(const char* filepath, const TextureSettings& settings) const { return TileFactory::GetFilepath(m_Tile) == filepath && m_Settings == settings; }
	inline bool Equivalent(const TileHandle& tile, const TextureSettings& settings) const { return m_Tile == tile && m_Settings == settings; }
	// TODO more sophisticated check for atlas? id may be the same for two distinct atlas objects, but they can have different subtiles.
	//bool Equivalent(const class Atlas& atlas, const TextureSettings& settings) const;
	inline bool IsValid() const { return m_RID > 0; }

public:
	Texture(const char* filepath, TextureSettings settings = {}, bool temporary_buffer = false);
	Texture(const TileHandle& tile, TextureSettings settings = {});
	//Texture(const class Atlas& atlas, TextureSettings settings = {});
	Texture(Texture&& texture) noexcept;
	Texture(const Texture& texture) = delete;
	~Texture();

	void Bind(TextureSlot slot = 0) const;
	void Unbind(TextureSlot slot = 0) const;

	void SetSettings(const TextureSettings& settings);

	inline int GetWidth() const { return TileFactory::GetWidth(m_Tile); }
	inline int GetHeight() const { return TileFactory::GetHeight(m_Tile); }
	inline TileHandle GetTileHandle() const { return m_Tile; }

	static const TextureSettings linear_settings;
	static const TextureSettings nearest_settings;
};
