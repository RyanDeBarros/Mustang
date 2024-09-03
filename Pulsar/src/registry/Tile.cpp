#include "Tile.h"

#include <stb/stb_image.h>
#include <nanosvg/nanosvg.h>
#include <nanosvg/nanosvgrast.h>

#include "Logger.inl"
#include "utils/Strings.h"

static void flip_vertically(unsigned char* buffer, int height, int stride)
{
	unsigned char* temp_row = new unsigned char[stride];
	for (int i = 0; i < height / 2; i++)
	{
		memcpy_s(temp_row, stride, buffer + i * stride, stride);
		memcpy_s(buffer + i * stride, stride, buffer + (height - 1 - i) * stride, stride);
		memcpy_s(buffer + (height - 1 - i) * stride, stride, temp_row, stride);
	}
	delete[] temp_row;
}

Tile::Tile(const char* filepath, float svg_scale)
	: m_ImageBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
	const auto& fext = file_extension_of(filepath);
	if (fext == "svg")
	{
		deletion_policy = TileDeletionPolicy::FROM_NEW;
		NSVGimage* image = nsvgParseFromFile(filepath, "px", 96);
		if (image)
		{
			NSVGrasterizer* rasterizer = nsvgCreateRasterizer();
			m_BPP = 4;
			if (svg_scale < 0.0f)
				svg_scale *= -1.0f;
			m_Width = static_cast<int>(svg_scale * image->width);
			m_Height = static_cast<int>(svg_scale * image->height);
			m_ImageBuffer = new unsigned char[m_Width * m_Height * 4];
			nsvgRasterize(rasterizer, image, 0.0f, 0.0f, svg_scale, m_ImageBuffer, m_Width, m_Height, m_Width * 4);
			nsvgDeleteRasterizer(rasterizer);
			nsvgDelete(image);

			flip_vertically(m_ImageBuffer, m_Height, m_Width * 4);
		}
		else
		{
			if (m_ImageBuffer)
			{
				delete[] m_ImageBuffer;
				m_ImageBuffer = nullptr;
			}
			Logger::LogError("SVG texture '" + std::string(filepath) + "' could not be loaded!");
			return;
		}
	}
	else
	{
		deletion_policy = TileDeletionPolicy::FROM_STBI;
		m_ImageBuffer = stbi_load(filepath, &m_Width, &m_Height, &m_BPP, 0);
		if (!m_ImageBuffer)
		{
			Logger::LogError("Texture '" + std::string(filepath) + "' could not be loaded!\n" + stbi_failure_reason());
			return;
		}
	}
}

Tile::Tile(unsigned char* heap_image_buffer, int width, int height, int bpp, TileDeletionPolicy deletion_policy)
	: m_ImageBuffer(heap_image_buffer), m_Width(width), m_Height(height), m_BPP(bpp), deletion_policy(deletion_policy)
{
}

Tile::Tile(Tile&& tile) noexcept
	: m_ImageBuffer(tile.m_ImageBuffer), m_Width(tile.m_Width), m_Height(tile.m_Height), m_BPP(tile.m_BPP), deletion_policy(tile.deletion_policy)
{
	tile.m_ImageBuffer = nullptr;
}

Tile& Tile::operator=(Tile&& tile) noexcept
{
	if (this == &tile)
		return *this;
	if (m_ImageBuffer)
		DeleteBuffer();
	m_ImageBuffer = tile.m_ImageBuffer;
	m_Width = tile.m_Width;
	m_Height = tile.m_Height;
	m_BPP = tile.m_BPP;
	deletion_policy = tile.deletion_policy;
	tile.m_ImageBuffer = nullptr;
	return *this;
}

Tile::~Tile()
{
	if (m_ImageBuffer)
	{
		DeleteBuffer();
		m_ImageBuffer = nullptr;
	}
}

void Tile::DeleteBuffer() const
{
	if (deletion_policy == TileDeletionPolicy::FROM_STBI) [[likely]]
		stbi_image_free(m_ImageBuffer);
	else if (deletion_policy == TileDeletionPolicy::FROM_NEW)
		delete[] m_ImageBuffer;
}
