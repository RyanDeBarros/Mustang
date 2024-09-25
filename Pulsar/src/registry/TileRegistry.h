#pragma once

#include <unordered_map>
#include <string>

#include "Tile.h"
#include "Handles.inl"

// TODO for all construct args, define constructors to initialize data members

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

class TileRegistry
{
	static TileHandle handle_cap;
	static std::unordered_map<TileHandle, Tile> registry;
	static std::unordered_map<TileConstructArgs_filepath, TileHandle> lookupMap_filepath;
	static std::unordered_map<TileConstructArgs_buffer, TileHandle> lookupMap_buffer;

	TileRegistry() = delete;
	TileRegistry(const TileRegistry&) = delete;
	TileRegistry(TileRegistry&&) = delete;
	~TileRegistry() = delete;

	friend class Renderer;
	static void Init();
	static void Terminate();

public:
	static TileHandle GetHandle(const TileConstructArgs_filepath& args);
	static TileHandle GetHandle(const TileConstructArgs_buffer& args);
	static Tile const* Get(TileHandle handle);
	static bool DestroyTile(TileHandle handle);

	static TileHandle RegisterTile(Tile&& emplace_tile);
	static unsigned char const* GetImageBuffer(TileHandle tile);
	static int GetWidth(TileHandle tile);
	static int GetHeight(TileHandle tile);
	static int GetBPP(TileHandle tile);
};
