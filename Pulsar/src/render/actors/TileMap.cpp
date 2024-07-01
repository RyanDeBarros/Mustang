#include "TileMap.h"

TileMap::TileMap(const TileHandle& atlas_handle, const TextureSettings& texture_settings, const ShaderHandle& shader, const ZIndex& z, const bool& visible)
{
	Tile* t = TileFactory::GetTileRef(atlas_handle);
	m_Atlas = dynamic_cast<Atlas*>(t);
	if (!m_Atlas)
		throw atlas_cast_error();
	for (size_t i = 0; i < m_Atlas->GetPlacements().size(); i++)
	{
		RectRender* rect_render = new RectRender(m_Atlas->SampleSubtile(i, texture_settings, shader, z, visible));
		ActorTesselation2D* tessel = new ActorTesselation2D(rect_render);
		m_Map.push_back({rect_render, tessel});
	}
}

TileMap::~TileMap()
{
	for (auto& [rect_render, tessel] : m_Map)
	{
		if (rect_render)
			delete rect_render;
		if (tessel)
			delete tessel;
	}
	m_Map.clear();
}

ActorPrimitive2D* const TileMap::operator[](const int& i)
{
	// TODO?
	return nullptr;
}

BufferCounter TileMap::PrimitiveCount() const
{
	BufferCounter count = 0;
	for (auto iter = m_Map.begin(); iter != m_Map.end(); iter++)
		count += iter->second->PrimitiveCount();
	return count;
}

void TileMap::RequestDraw(CanvasLayer* canvas_layer)
{
	for (auto& tessel : m_Map)
		tessel.second->RequestDraw(canvas_layer);
}

ActorTesselation2D* const TileMap::TesselationRef(const int& i) const
{
	if (i >= 0 && i < m_Map.size())
		return m_Map[i].second;
	else return nullptr;
}
