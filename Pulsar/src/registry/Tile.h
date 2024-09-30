#pragma once

#include <string>

#include "Registry.inl"
#include "Handles.inl"

struct TileRect
{
	int x, y, w, h;
};

enum class TileDeletionPolicy : unsigned char
{
	FROM_STBI,
	FROM_NEW,
	FROM_EXTERNAL
};

struct TileConstructArgs_filepath
{
	std::string filepath;
	float svg_scale = 1.0f;
	bool flip_vertically = true;

	TileConstructArgs_filepath(const std::string& filepath, float svg_scale = 1.0f, bool flip_vertically = true)
		: filepath(filepath), svg_scale(svg_scale), flip_vertically(flip_vertically) {}
	TileConstructArgs_filepath(std::string&& filepath, float svg_scale = 1.0f, bool flip_vertically = true)
		: filepath(std::move(filepath)), svg_scale(svg_scale), flip_vertically(flip_vertically) {}

	bool operator==(const TileConstructArgs_filepath& args) const
	{
		return filepath == args.filepath && svg_scale == args.svg_scale && flip_vertically == args.flip_vertically;
	}
};

template<>
struct std::hash<TileConstructArgs_filepath>
{
	size_t operator()(const TileConstructArgs_filepath& args) const
	{
		return hash<std::string>{}(args.filepath) ^ (hash<float>{}(args.svg_scale) << 1) ^ (hash<bool>{}(args.flip_vertically) << 2);
	}
};

struct TileConstructArgs_buffer
{
	unsigned char* image_buffer;
	int width;
	int height;
	int bpp;
	TileDeletionPolicy deletion_policy;
	bool flip_vertically = false;

	TileConstructArgs_buffer(unsigned char* image_buffer, int width, int height, int bpp, TileDeletionPolicy deletion_policy, bool flip_vertically = false)
		: image_buffer(image_buffer), width(width), height(height), bpp(bpp), deletion_policy(deletion_policy), flip_vertically(flip_vertically) {}

	bool operator==(const TileConstructArgs_buffer& args) const
	{
		return image_buffer == args.image_buffer && width == args.width && height == args.height
			&& bpp == args.bpp && deletion_policy == args.deletion_policy && flip_vertically == args.flip_vertically;
	}
};

template<>
struct std::hash<TileConstructArgs_buffer>
{
	size_t operator()(const TileConstructArgs_buffer& args) const
	{
		// NOTE image buffer is most important. There aren't many Tiles where the image buffers are equal but the other properties are different
		// ==> low collisions but high hashing speed.
		// TODO do the above for other construct args hashes
		return hash<unsigned char*>{}(args.image_buffer);
	}
};

class Tile
{
	friend class TileRegistry;
	friend class Texture;
	friend class NonantTile;
	unsigned char* m_ImageBuffer;
	int m_Width, m_Height, m_BPP;
	TileDeletionPolicy deletion_policy;
	
	Tile() : m_Width(0), m_Height(0), m_BPP(0), deletion_policy(TileDeletionPolicy::FROM_EXTERNAL), m_ImageBuffer(nullptr) {}
	void DeleteBuffer() const;

public:
	Tile(const TileConstructArgs_filepath& args);
	Tile(const TileConstructArgs_buffer& args);
	Tile(const Tile&) = delete;
	Tile(Tile&& tile) noexcept;
	Tile& operator=(Tile&& tile) noexcept;
	~Tile();

	operator bool() const { return m_ImageBuffer != nullptr; }

	unsigned char const* GetImageBuffer() const { return m_ImageBuffer; }
	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	int GetBPP() const { return m_BPP; }
};

class TileRegistry : public Registry<Tile, TileHandle, TileConstructArgs_filepath, TileConstructArgs_buffer>
{
public:
	unsigned char const* GetImageBuffer(TileHandle tile) { Tile const* t = Get(tile); return t ? t->m_ImageBuffer : nullptr; }
	int GetWidth(TileHandle tile) { Tile const* t = Get(tile); return t ? t->m_Width : 0; }
	int GetHeight(TileHandle tile) { Tile const* t = Get(tile); return t ? t->m_Height : 0; }
	int GetBPP(TileHandle tile) { Tile const* t = Get(tile); return t ? t->m_BPP : 0; }
};
