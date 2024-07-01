#include "Atlas.h"

#include <queue>
#include <vector>
#include <unordered_map>

#include "Macros.h"
#include "TileFactory.h"
#include "render/actors/RectRender.h"
#include "render/actors/ActorTesselation.h"

Atlas::Atlas(std::vector<TileHandle>& tiles, int width, int height, int border)
	: m_Border(border)
{
	m_BPP = Atlas::BPP;
	RectPack(tiles, width, height);
	m_BufferSize = Atlas::BPP * m_Width * m_Height;
	m_ImageBuffer = new unsigned char[m_BufferSize](0);
	PlaceTiles();
}

Atlas::Atlas(std::vector<TileHandle>&& tiles, int width, int height, int border)
	: m_Border(border)
{
	m_BPP = Atlas::BPP;
	RectPack(tiles, width, height);
	m_BufferSize = Atlas::BPP * m_Width * m_Height;
	m_ImageBuffer = new unsigned char[m_BufferSize](0);
	PlaceTiles();
}

Atlas::Atlas(const char* texture_filepath, const std::vector<Placement>& placements, int border)
	: Tile(texture_filepath)
{
	ASSERT(m_BPP == Atlas::BPP);
	m_BufferSize = Atlas::BPP * m_Width * m_Height;
	m_Border = border;
	m_Placements = placements;
}

Atlas::Atlas(Atlas&& atlas) noexcept
	: Tile(std::move(atlas)), m_Border(atlas.m_Border), m_BufferSize(atlas.m_BufferSize), m_Placements(atlas.m_Placements)
{
}

Atlas::Atlas(const Atlas* const atlas)
{
	m_Filepath = atlas->m_Filepath;
	m_Width = atlas->m_Width;
	m_Height = atlas->m_Height;
	m_BPP = atlas->m_BPP;
	m_Border = atlas->m_Border;
	m_BufferSize = atlas->m_BufferSize;
	m_Placements = atlas->m_Placements;
	m_ImageBuffer = new unsigned char[m_BufferSize](0);
	memcpy_s(m_ImageBuffer, m_BufferSize, atlas->m_ImageBuffer, m_BufferSize);
}

Atlas::~Atlas()
{
	if (m_ImageBuffer)
	{
		delete[] m_ImageBuffer;
		m_ImageBuffer = nullptr;
	}
}

bool Atlas::operator==(const Atlas& other) const
{
	return m_Filepath == other.m_Filepath && m_Width == other.m_Width && m_Height == other.m_Height && m_BPP == other.m_BPP
			&& m_Border == other.m_Border && m_BufferSize == other.m_BufferSize && m_BufferSize == m_BufferSize && memcmp(m_ImageBuffer, other.m_ImageBuffer, m_BufferSize);
}

static int min_bound(const std::vector<TileHandle>& tiles, const int& border)
{
	int bound = border;
	for (const TileHandle& tile : tiles)
		bound += std::max(TileFactory::GetWidth(tile), TileFactory::GetHeight(tile)) + border;
	return bound;
}

bool Atlas::Equivalent(std::vector<TileHandle>& tiles, int width, int height, int border) const
{
	if (m_Border != border)
		return false;
	int bound = min_bound(tiles, m_Border);
	if (width <= 0)
		width = bound;
	if (height <= 0)
		height = bound;
	if (m_Width != width || m_Height != height)
		return false;

	std::unordered_map<TileHandle, int> occurences;
	for (const auto& tile : tiles)
		occurences[tile]++;

	for (auto iter = m_Placements.begin(); iter != m_Placements.end(); iter++)
	{
		auto leftover = occurences[iter->tile]--;
		if (leftover < 0)
			return false;
		else if (leftover == 0)
			occurences.erase(iter->tile);
	}
	return occurences.empty();
}

bool Atlas::Equivalent(std::vector<TileHandle>&& tiles, int width, int height, int border) const
{
	if (m_Border != border)
		return false;
	int bound = min_bound(tiles, m_Border);
	if (width <= 0)
		width = bound;
	if (height <= 0)
		height = bound;
	if (m_Width != width || m_Height != height)
		return false;

	std::unordered_map<TileHandle, int> occurences;
	for (const auto& tile : tiles)
		occurences[tile]++;

	for (auto iter = m_Placements.begin(); iter != m_Placements.end(); iter++)
	{
		auto leftover = occurences[iter->tile]--;
		if (leftover < 0)
			return false;
		else if (leftover == 0)
			occurences.erase(iter->tile);
	}
	return occurences.empty();
}

bool Atlas::Equivalent(const char* texture_filepath, const std::vector<Placement>& placements, int border) const
{
	return m_Filepath == texture_filepath && m_Border == border && m_Placements == placements;
}

struct Subsection
{
	int x, y, w, h, rw = 0, rh = 0;

	Placement insert(TileHandle tile, int rect_w, int rect_h)
	{
		if (rect_w <= w && rect_h <= h)
		{
			rw = rect_w;
			rh = rect_h;
			return { tile, x, y, rw, rh, false };
		}
		else if (rect_h <= w && rect_w <= h)
		{
			rw = rect_h;
			rh = rect_w;
			return { tile, x, y, rw, rh, true };
		}
		else return { 0, 0, 0, 0, 0, false };
	}

	std::pair<Subsection, Subsection> split() const
	{
		if (rw > 0 && rh > 0)
		{
			if (w - rw >= h - rh)
			{
				Subsection s1{ x + rw, y, w - rw, h };
				Subsection s2{ x, y + rh, rw, h - rh };
				return { s1, s2 };
			}
			else
			{
				Subsection s1{ x + rw, y, w - rw, rh };
				Subsection s2{ x, y + rh, w, h - rh };
				return { s1, s2 };
			}
		}
		else return { {-1}, {} };
	}

	bool merge(const Subsection& other)
	{
		if (w == other.w && x == other.x)
		{
			if (y + h == other.y)
			{
				h += other.h;
				return true;
			}
			else if (y == other.y + other.h)
			{
				y -= other.h;
				h += other.h;
				return true;
			}
		}
		else if (h == other.h && y == other.y)
		{
			if (x + w == other.x)
			{
				w += other.w;
				return true;
			}
			else if (x == other.x + other.w)
			{
				x -= other.w;
				w += other.w;
				return true;
			}
		}
		return false;
	}

	static void merge(std::vector<Subsection>& subsections)
	{
		// TODO optimize so that merge only happens when split occurs such that adjacent subsections can merge.
		size_t i = 0;
		while (i < subsections.size())
		{
			Subsection& orig = subsections[i];
			size_t j = i + 1;
			while (j < subsections.size())
			{
				if (orig.merge(subsections[j]))
				{
					subsections.erase(subsections.begin() + j);
				}
				else j++;
			}
			i++;
		}
	}

};

void Atlas::RectPack(std::vector<TileHandle>& tiles, const int& width, const int& height)
{
	int bound = min_bound(tiles, m_Border);
	m_Width = width > 0 ? width : bound;
	m_Height = height > 0 ? height : bound;
	
	std::sort(tiles.begin(), tiles.end(), [](const TileHandle& a, const TileHandle& b)
	{
		return std::max(TileFactory::GetWidth(a), TileFactory::GetHeight(a))
				> std::max(TileFactory::GetWidth(b), TileFactory::GetHeight(b));
	});

	m_Placements.reserve(tiles.size());
	Subsection root{ 0, 0, m_Width, m_Height };
	auto comparator = [](const Subsection& l, const Subsection& r) { return std::max(l.w, l.h) < std::max(r.w, r.h); };
	std::priority_queue<Subsection, std::vector<Subsection>, decltype(comparator)> subsections(comparator);
	subsections.push(root);
	std::vector<Subsection> tried_subsections;
	tried_subsections.reserve(tiles.size() + 1);

	for (const TileHandle& tile : tiles)
	{
		tried_subsections.clear();
		bool fits = false;
		while (!subsections.empty())
		{
			Subsection subsection = subsections.top();
			subsections.pop();
			Placement result = subsection.insert(tile, TileFactory::GetWidth(tile) + m_Border, TileFactory::GetHeight(tile) + m_Border);
			if (result.tile > 0)
			{
				m_Placements.push_back(result);
				auto split = subsection.split();
				if (split.first.x >= 0)
				{
					subsections.push(split.first);
					subsections.push(split.second);
					std::vector<Subsection> old_subsections;
					while (!subsections.empty())
					{
						old_subsections.push_back(subsections.top());
						subsections.pop();
					}
					Subsection::merge(old_subsections);
					while (!old_subsections.empty())
					{
						subsections.push(old_subsections[0]);
						old_subsections.erase(old_subsections.begin());
					}
				}
				else
				{
					subsections.push(subsection);
				}
				fits = true;
				break;
			}
			else
			{
				tried_subsections.push_back(subsection);
			}
		}
		for (const auto& subsection : tried_subsections)
		{
			subsections.push(subsection);
		}
		if (!fits)
		{
			m_Placements.push_back({0, -1, -1, -1, -1, false});
		}
	}
}

void Atlas::PlaceTiles()
{
	for (const Placement& placement : m_Placements)
	{
		if (placement.tile == 0)
			continue;
		const unsigned char* image_buffer = TileFactory::GetImageBuffer(placement.tile);
		const auto width = placement.w - m_Border;
		const auto height = placement.h - m_Border;
		const auto bpp = TileFactory::GetBPP(placement.tile);
		for (size_t h = 0; h < height; h++)
		{
			for (size_t w = 0; w < width; w++)
			{
				for (unsigned char c = 0; c < bpp && c < Atlas::BPP; c++)
				{
					if (placement.r)
					{
						m_ImageBuffer[(placement.x + m_Border + h + (placement.y + m_Border + w) * m_Width) * Atlas::BPP + c] = image_buffer[(w + h * height) * bpp + c];
					}
					else
					{
						m_ImageBuffer[(placement.x + m_Border + w + (placement.y + m_Border + h) * m_Width) * Atlas::BPP + c] = image_buffer[(w + h * width) * bpp + c];
					}
				}
				for (unsigned char c = bpp; c < Atlas::BPP; c++)
				{
					if (placement.r)
					{
						m_ImageBuffer[(placement.x + m_Border + h + (placement.y + m_Border + w) * m_Width) * Atlas::BPP + c] = 255;
					}
					else
					{
						m_ImageBuffer[(placement.x + m_Border + w + (placement.y + m_Border + h) * m_Width) * Atlas::BPP + c] = 255;
					}
				}
			}
		}
	}
}

RectRender Atlas::SampleSubtile(const size_t& index, const TextureSettings& texture_settings, const ShaderHandle& shader, const ZIndex& z, const bool& visible) const
{
	if (index >= m_Placements.size() || m_Placements[index].x < 0)
		return { 0, {}, 0, 0, false };
	RectRender actor(TextureFactory::GetHandle(TileFactory::GetAtlasHandle(this), texture_settings), {}, shader, z, visible);
	const Placement& rect = m_Placements[index];
	if (rect.r)
		actor.CropToRect({ rect.x + m_Border, rect.y + m_Border, rect.h - m_Border, rect.w - m_Border }, m_Width, m_Height);
	else
		actor.CropToRect({ rect.x + m_Border, rect.y + m_Border, rect.w - m_Border, rect.h - m_Border }, m_Width, m_Height);
	actor.SetPivot(0.5, 0.5);
	actor.SetScale((rect.w - m_Border) / (float)m_Width, (rect.h - m_Border) / (float)m_Height);
	return actor;
}
