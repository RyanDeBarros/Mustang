#include "Atlas.h"

#include <queue>

#include "TileFactory.h"

unsigned int id_count = 1;

// TODO use factory? so that id is not incremented if atlas is loaded from same file. This would probably be in a different constructor. Perhaps not a full factory class, but a static map that maps atlas filepaths to ids.
Atlas::Atlas(std::vector<TileHandle>& tiles, const int& width, const int& height)
	: m_AtlasBuffer(nullptr), id(id_count++)
{
	RectPack(tiles, width, height);
	m_BufferSize = Atlas::BPP * m_Width * m_Height; // 4 is BPP
	m_AtlasBuffer = new unsigned char[m_BufferSize](0);
	PlaceTiles();
}

Atlas::~Atlas()
{
	if (m_AtlasBuffer)
		delete m_AtlasBuffer;
}

static int min_bound(const std::vector<TileHandle>& tiles)
{
	int bound = 0;
	for (const TileHandle& tile : tiles)
		bound += std::max(TileFactory::GetWidth(tile), TileFactory::GetHeight(tile));
	return bound;
}

struct Placement
{
	TileHandle tile;
	int x, y, w, h;
	bool r;
};

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
			if (w - rw > h - rh)
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
};

void Atlas::RectPack(std::vector<TileHandle>& tiles, const int& width, const int& height)
{
	int bound = min_bound(tiles);
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
			Placement result = subsection.insert(tile, TileFactory::GetWidth(tile), TileFactory::GetHeight(tile));
			if (result.tile > 0)
			{
				m_Placements.push_back(result);
				auto split = subsection.split();
				if (split.first.x >= 0)
				{
					subsections.push(split.first);
					subsections.push(split.second);
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
		const auto width = placement.w;
		const auto height = placement.h;
		const auto bpp = TileFactory::GetBPP(placement.tile);
		for (size_t h = 0; h < height; h++)
		{
			for (size_t w = 0; w < width; w++)
			{
				for (unsigned char c = 0; c < bpp && c < Atlas::BPP; c++)
				{
					if (placement.r)
					{
						m_AtlasBuffer[(placement.x + h + (placement.y + w) * m_Width) * Atlas::BPP + c] = image_buffer[(w + h * height) * bpp + c];
					}
					else
					{
						m_AtlasBuffer[(placement.x + w + (placement.y + h) * m_Width) * Atlas::BPP + c] = image_buffer[(w + h * width) * bpp + c];
					}
				}
				for (unsigned char c = bpp; c < Atlas::BPP; c++)
				{
					m_AtlasBuffer[(placement.x + w + (placement.y + h) * m_Width) * Atlas::BPP + c] = 255;
				}
			}
		}
	}
}
