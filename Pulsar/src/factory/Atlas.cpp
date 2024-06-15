#include "Atlas.h"

#include "TileFactory.h"

int SortTileSet::operator() (const std::pair<TileHandle, POTwrapper>& p1, const std::pair<TileHandle, POTwrapper>& p2) const
{
	return (p2.second.potW * p2.second.potH) - (p1.second.potW * p1.second.potH);
}

int Atlas::UpperPot(int n)
{
	int pot = 1;
	while (pot < n) pot <<= 1;
	return pot;
}

int Atlas::LowerPot(int n)
{
	int pot = 1;
	while (n > 1)
	{
		n >>= 1;
		pot <<= 1;
	}
	return pot;
}

POTwrapper Atlas::Wrap(const TileRect& rect)
{
	return { rect, UpperPot(rect.w), UpperPot(rect.h) };
}

Atlas::Atlas(const int& width, const int& height)
	: m_AtlasBuffer(nullptr), m_Width(width), m_Height(height)
{
	m_AtlasBuffer = new unsigned char[BufferSize()];
}

Atlas::~Atlas()
{
	if (m_AtlasBuffer)
		delete m_AtlasBuffer;
}

void Atlas::Insert(const TileHandle& tile)
{
	const Tile* t = TileFactory::GetConstTileRef(tile);
	if (!t)
		return;
	m_TileSet.insert({ tile, Wrap({ 0, 0, t->m_Width, t->m_Height }) });
}

void Atlas::Pack()
{
	memset(m_AtlasBuffer, 0, BufferSize());
	int x = 0, y = 0;
	for (auto iter = m_TileSet.begin(); iter != m_TileSet.end(); iter++)
	{
		// TODO rect packing algorithm
	}
}
