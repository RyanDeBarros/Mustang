#include "TileMap.h"

#include <algorithm>

TileMap::TileMap(TileHandle atlas_handle, const TextureSettings& texture_settings, ShaderHandle shader, ZIndex z, bool visible)
	: ProteanActor2D(z)
{
	Tile* t = TileFactory::GetTileRef(atlas_handle);
	m_Atlas = dynamic_cast<Atlas*>(t);
	if (!m_Atlas)
		throw atlas_cast_error();
	for (TileMapIndex i = 0; i < m_Atlas->GetPlacements().size(); i++)
	{
		std::unique_ptr<RectRender> rect_render(std::make_unique<RectRender>(m_Atlas->SampleSubtile(i, texture_settings, shader, 0, visible)));
		std::shared_ptr<ActorTesselation2D> tessel(std::make_shared<ActorTesselation2D>(rect_render.get()));
		m_ProteanLinker.Attach(tessel->RefProteanLinker());
		m_Map.push_back({ std::move(rect_render), std::move(tessel) });
	}
	m_Ordering = Permutation(m_Atlas->GetPlacements().size());
}

TileMap::~TileMap()
{
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
	m_Map[tessel].tessel->PushBackStatic({ { {posX * m_Map[tessel].rectRender->GetUVWidth(), posY * m_Map[tessel].rectRender->GetUVHeight()} } });
}

ActorTesselation2D* const TileMap::TesselationRef(TileMapIndex i) const
{
	if (i >= 0 && i < m_Ordering.size())
		return m_Map[i].tessel.get();
	else return nullptr;
}
