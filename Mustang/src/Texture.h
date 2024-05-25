#pragma once

#include <GL/glew.h>

#include "Typedefs.h"

enum class MinFilter : GLint
{
	Nearest = GL_NEAREST,
	Linear = GL_LINEAR,
	NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
	LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
	NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
	LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
};

enum class MagFilter : GLint
{
	Nearest = GL_NEAREST,
	Linear = GL_LINEAR
};

enum class TextureWrap : GLint
{
	ClampToEdge = GL_CLAMP_TO_EDGE,
	ClampToBorder = GL_CLAMP_TO_BORDER,
	MirroredRepeat = GL_MIRRORED_REPEAT,
	Repeat = GL_REPEAT,
	MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE
};

struct TextureSettings
{
	MinFilter min_filter = MinFilter::Linear;
	MagFilter mag_filter = MagFilter::Linear;
	TextureWrap wrap_s = TextureWrap::ClampToEdge;
	TextureWrap wrap_t = TextureWrap::ClampToEdge;

	bool operator==(const TextureSettings& other) const
	{
		return min_filter == other.min_filter && mag_filter == other.mag_filter && wrap_s == other.wrap_s && wrap_t == other.wrap_t;
	}
};

class Texture
{
	RID m_RID;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
public:
	Texture(const char* filepath, TextureSettings settings = TextureSettings(), GLint lod_level = 0);
	~Texture();

	void Bind(TextureSlot slot = 0) const;
	void Unbind(TextureSlot slot = 0) const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }

	inline bool IsValid() const { return m_RID > 0; }
};
