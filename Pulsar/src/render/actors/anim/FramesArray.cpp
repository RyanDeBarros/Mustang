#include "FramesArray.h"

#include <stb/stb_image.h>
#include <fstream>

#include "Logger.inl"
#include "registry/TextureRegistry.h"

FramesArray::FramesArray(const char* gif_filepath, const TextureSettings& settings, unsigned short starting_index, bool temporary_buffer)
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
	m_Frames = Array<TileHandle>(num_frames, 0);
	
	if (temporary_buffer)
	{
		for (size_t i = 0; i < num_frames; i++)
		{
			unsigned char* image = new unsigned char[image_size];
			memcpy_s(image, image_size, stbi_buffer + image_size * i, image_size);
			m_Frames[i] = TextureRegistry::RegisterTexture(Texture(Tile(image, width, height, bpp), settings));
		}
	}
	else
	{
		for (size_t i = 0; i < num_frames; i++)
		{
			unsigned char* image = new unsigned char[image_size];
			memcpy_s(image, image_size, stbi_buffer + image_size * i, image_size);
			TileHandle tile = TileRegistry::RegisterTile(Tile(image, width, height, bpp));
			m_Frames[i] = TextureRegistry::GetHandle(TextureConstructArgs_tile{ tile, 0, settings });
		}
	}

	stbi_image_free(stbi_buffer);
	Select(starting_index);
}

FramesArray::FramesArray(Array<TextureHandle>&& frames, unsigned short starting_index)
	: m_Frames(frames)
{
	Select(starting_index);
}
