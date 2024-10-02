#include "Atlas.h"

#include <queue>
#include <vector>

#include "Macros.h"
#include "render/actors/RectRender.h"
#include "render/actors/ActorTesselation.h"

Atlas::Atlas(std::vector<TileHandle>& tiles, int width, int height, int border)
	: m_Border(border)
{
	RectPack(tiles, width, height);
	unsigned char* image_buffer = new unsigned char[Atlas::BPP * width * height](0);
	PlaceTiles(image_buffer, width);
	Tile tile(TileConstructArgs_buffer(image_buffer, width, height, Atlas::BPP, TileDeletionPolicy::FROM_NEW));
	m_Tile = Renderer::Tiles().Register(std::move(tile));
	if (m_Tile == 0)
		throw null_handle_error();
}

Atlas::Atlas(const std::string& texture_filepath, const std::vector<Placement>& placements, int border)
	: m_Placements(placements), m_Border(border)
{
	m_Tile = Renderer::Tiles().GetHandle({ texture_filepath });
}

Atlas::Atlas(const std::string& texture_filepath, std::vector<Placement>&& placements, int border)
	: m_Placements(std::move(placements)), m_Border(border)
{
	m_Tile = Renderer::Tiles().GetHandle({ texture_filepath });
}

static int min_bound(const std::vector<TileHandle>& tiles, const int& border)
{
	int bound = border;
	for (const TileHandle& tile : tiles)
		bound += std::max(Renderer::Tiles().GetWidth(tile), Renderer::Tiles().GetHeight(tile)) + border;
	return bound;
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

void Atlas::RectPack(std::vector<TileHandle>& tiles, int width, int height)
{
	int bound = min_bound(tiles, m_Border);
	if (width <= 0)
		width = bound;
	if (height <= 0)
		height = bound;

	std::sort(tiles.begin(), tiles.end(), [](const TileHandle& a, const TileHandle& b)
	{
		return std::max(Renderer::Tiles().GetWidth(a), Renderer::Tiles().GetHeight(a))
				> std::max(Renderer::Tiles().GetWidth(b), Renderer::Tiles().GetHeight(b));
	});

	m_Placements.reserve(tiles.size());
	Subsection root{ 0, 0, width, height };
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
			Placement result = subsection.insert(tile, Renderer::Tiles().GetWidth(tile) + m_Border, Renderer::Tiles().GetHeight(tile) + m_Border);
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
			subsections.push(subsection);
		if (!fits)
			m_Placements.push_back({0, -1, -1, -1, -1, false});
	}
}

void Atlas::PlaceTiles(unsigned char* image_buffer, int atlas_width)
{
	for (const Placement& placement : m_Placements)
	{
		if (placement.tile == 0)
			continue;
		unsigned char const* p_buffer = Renderer::Tiles().GetImageBuffer(placement.tile);
		if (!p_buffer)
			continue;
		const auto p_width = placement.w - m_Border;
		const auto p_height = placement.h - m_Border;
		const auto p_bpp = Renderer::Tiles().GetBPP(placement.tile);
		for (int h = 0; h < p_height; h++)
		{
			for (int w = 0; w < p_width; w++)
			{
				for (unsigned char c = 0; c < p_bpp && c < Atlas::BPP; c++)
				{
					if (placement.r)
						image_buffer[(placement.x + m_Border + h + (placement.y + m_Border + w) * atlas_width) * Atlas::BPP + c]
							= p_buffer[(w + h * p_height) * p_bpp + c];
					else
						image_buffer[(placement.x + m_Border + w + (placement.y + m_Border + h) * atlas_width) * Atlas::BPP + c]
							= p_buffer[(w + h * p_width) * p_bpp + c];
				}
				for (unsigned char c = p_bpp; c < Atlas::BPP; c++)
				{
					if (placement.r)
						image_buffer[(placement.x + m_Border + h + (placement.y + m_Border + w) * atlas_width) * Atlas::BPP + c] = 255;
					else
						image_buffer[(placement.x + m_Border + w + (placement.y + m_Border + h) * atlas_width) * Atlas::BPP + c] = 255;
				}
			}
		}
	}
}

RectRender Atlas::SampleSubtile(size_t index, const TextureSettings& texture_settings, TextureVersion texture_version,
	const glm::vec2& pivot, ShaderHandle shader, ZIndex z, FickleType fickle_type, bool visible) const
{
	if (index >= m_Placements.size() || m_Placements[index].x < 0)
		return RectRender(0, {}, 0, 0, fickle_type, false);
	RectRender actor(Renderer::Textures().GetHandle(TextureConstructArgs_tile(m_Tile, texture_version, texture_settings)),
		pivot, shader == Renderer::Shaders().Standard(), z, fickle_type, visible);
	const Placement& rect = m_Placements[index];
	int width = Renderer::Tiles().GetWidth(m_Tile);
	int height = Renderer::Tiles().GetHeight(m_Tile);
	if (rect.r)
		actor.CropToRect({ rect.x + m_Border, rect.y + m_Border, rect.h - m_Border, rect.w - m_Border }, width, height);
	else
		actor.CropToRect({ rect.x + m_Border, rect.y + m_Border, rect.w - m_Border, rect.h - m_Border }, width, height);
	actor.SetPivot(0.5, 0.5);
	if (actor.Fickler().transformable)
	{
		*actor.Fickler().Scale() = { (rect.w - m_Border) / static_cast<float>(width), (rect.h - m_Border) / static_cast<float>(height) };
		actor.Fickler().SyncRS();
	}
	return actor;
}
