#include "TileMap.h"

#include <algorithm>

TileMap::TileMap(TileHandle atlas_handle, const TextureSettings& texture_settings, ShaderHandle shader, ZIndex z, bool visible)
	: ActorRenderBase2D(z), m_Transform(std::make_shared<TransformableProxy2D>()), m_Transformer(m_Transform)
{
	Tile* t = TileFactory::GetTileRef(atlas_handle);
	m_Atlas = dynamic_cast<Atlas*>(t);
	if (!m_Atlas)
		throw atlas_cast_error();
	for (TileMapIndex i = 0; i < m_Atlas->GetPlacements().size(); i++)
	{
		std::shared_ptr<RectRender> rect_render(new RectRender(m_Atlas->SampleSubtile(i, texture_settings, shader, 0, visible)));
		std::shared_ptr<ActorTesselation2D> tessel(new ActorTesselation2D(rect_render));
		m_Map.push_back({ std::move(rect_render), tessel });
		m_Transformer.PushBackGlobal(tessel->Transform());
	}
	m_Ordering = Permutation(m_Atlas->GetPlacements().size());
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
	for (TileMapIndex i = 0; i < m_Ordering.size(); i++)
		m_Map[m_Ordering[i]].tessel->RequestDraw(canvas_layer);
}

bool TileMap::SetOrdering(const Permutation& permutation)
{
	if (m_Ordering.size() != permutation.size())
		return false;
	m_Ordering = permutation;
	return true;
}

void TileMap::Insert(TileMapIndex tessel, float posX, float posY)
{
	m_Map[tessel].tessel->PushBackLocal({ {posX * m_Map[tessel].rectRender->GetUVWidth(), posY * m_Map[tessel].rectRender->GetUVHeight()} });
}

ActorTesselation2D* const TileMap::TesselationRef(TileMapIndex i) const
{
	if (i >= 0 && i < m_Ordering.size())
		return m_Map[i].tessel.get();
	else return nullptr;
}
