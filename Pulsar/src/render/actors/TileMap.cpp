#include "TileMap.h"

#include <algorithm>

TileMap::TileMap(const TileHandle& atlas_handle, const TextureSettings& texture_settings, const ShaderHandle& shader, const ZIndex& z, const bool& visible)
	: m_Z(z)
{
	Tile* t = TileFactory::GetTileRef(atlas_handle);
	m_Atlas = dynamic_cast<Atlas*>(t);
	if (!m_Atlas)
		throw atlas_cast_error();
	for (size_t i = 0; i < m_Atlas->GetPlacements().size(); i++)
	{
		RectRender* rect_render = new RectRender(m_Atlas->SampleSubtile(i, texture_settings, shader, 0, visible));
		ActorTesselation2D* tessel = new ActorTesselation2D(rect_render);
		m_Map.push_back({rect_render, tessel});
	}
	m_Ordering = Permutation(m_Atlas->GetPlacements().size());
}

TileMap::~TileMap()
{
	for (TMElement& element : m_Map)
	{
		if (element.rectRender)
			delete element.rectRender;
		if (element.tessel)
			delete element.tessel;
	}
	m_Map.clear();
}

ActorPrimitive2D* const TileMap::operator[](const int& i)
{
	// TODO? make TileMap just an ActorRenderBase, not ActorSequencer?
	return nullptr;
}

BufferCounter TileMap::PrimitiveCount() const
{
	BufferCounter count = 0;
	for (auto iter = m_Map.begin(); iter != m_Map.end(); iter++)
		count += iter->tessel->PrimitiveCount();
	return count;
}

void TileMap::RequestDraw(CanvasLayer* canvas_layer)
{
	for (size_t i = 0; i < m_Ordering.size(); i++)
		m_Map[m_Ordering[i]].tessel->RequestDraw(canvas_layer);
}

bool TileMap::SetOrdering(const Permutation& permutation)
{
	if (m_Ordering.size() != permutation.size())
		return false;
	m_Ordering = permutation;
	return true;
}

void TileMap::Insert(const size_t& tessel, float posX, float posY)
{
	float uvw = m_Map[tessel].rectRender->GetUVWidth() * m_Transform.scale.x;
	float uvh = m_Map[tessel].rectRender->GetUVHeight() * m_Transform.scale.y;
	float xi = m_Transform.position.x + posX * uvw;
	float yi = m_Transform.position.y + posY * uvh;
	float x = xi * glm::cos(m_Transform.rotation) - yi * glm::sin(m_Transform.rotation);
	float y = xi * glm::sin(m_Transform.rotation) + yi * glm::cos(m_Transform.rotation);
	m_Map[tessel].tessel->Insert({ {x, y}, m_Transform.rotation, {m_Transform.scale.x, m_Transform.scale.y}});
}

ActorTesselation2D* const TileMap::TesselationRef(const size_t& i) const
{
	if (i >= 0 && i < m_Ordering.size())
		return m_Map[i].tessel;
	else return nullptr;
}
