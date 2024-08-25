#include "FramesArray.h"

#include <stb/stb_image.h>
#include <fstream>

#include "Logger.inl"
#include "factory/TextureRegistry.h"

FramesArray::FramesArray(const char* gif_filepath, const TextureSettings& settings, float framelength, unsigned short starting_index)
{
	std::ifstream file(gif_filepath, std::ios::binary | std::ios::ate);
	if (!file)
	{
		Logger::LogError(std::string("Failed to open GIF file: ") + gif_filepath);
		return;
	}

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	
	unsigned char* full_buffer = new unsigned char[size];
	if (!file.read(reinterpret_cast<char*>(full_buffer), size))
	{
		Logger::LogError(std::string("Failed to read GIF file : ") + gif_filepath);
		delete[] full_buffer;
		return;
	}

	int width, height, num_frames, bpp;
	int* delays;
	unsigned char* stbi_buffer = stbi_load_gif_from_memory(full_buffer, static_cast<int>(size), &delays, &width, &height, &num_frames, &bpp, 0);
	delete[] full_buffer;
	if (!stbi_buffer)
	{
		Logger::LogError(std::string("Failed to load GIF: ") + gif_filepath);
		return;
	}

	size_t stride = static_cast<size_t>(width) * bpp;
	size_t image_size = stride * height;
	size_t full_size = image_size * num_frames;
	m_Frames = Array<TextureHandle>(num_frames);
	for (size_t i = 0; i < num_frames; i++)
	{
		unsigned char* image = new unsigned char[image_size];
		memcpy_s(image, image_size, stbi_buffer + image_size * i, image_size);
		TileHandle tile = TileRegistry::RegisterTile(Tile(image, width, height, bpp));
		m_Frames[i] = TextureRegistry::GetHandle(TextureConstructArgs_tile{ tile, 0, settings });
		// TODO instead of array of texture handles, use array of texture pointers. That way, there could be a temporary buffer version of FramesArray constructor that doesn't save the buffer in a tile. Instead of using TextureRegistry in that case, use heap pointers with a bool that remembers to delete them in FramesArray destructor. In that case, custom copy/move constructor/assignment operators will need to be implemented.
	}

	stbi_image_free(stbi_buffer);
	Select(starting_index);
}

FramesArray::FramesArray(Array<TextureHandle>&& frames, float framelength, unsigned short starting_index)
	: m_Frames(std::move(frames))
{
	Select(starting_index);
}
