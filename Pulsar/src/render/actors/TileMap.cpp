#include "TileMap.h"

#include <algorithm>

TileMap::TileMap(const std::shared_ptr<const Atlas>& atlas, const TextureSettings& texture_settings, TextureVersion texture_version,
	const glm::vec2& pivot, ShaderHandle shader, ZIndex z, FickleType fickle_type, bool visible)
	: FickleActor2D(fickle_type, z), m_Atlas(atlas)
{
	if (!m_Atlas)
		throw null_pointer_error();
	for (TileMapIndex i = 0; i < m_Atlas->GetPlacements().size(); i++)
	{
		std::unique_ptr<RectRender> rect_render(std::make_unique<RectRender>(
			m_Atlas->SampleSubtile(i, texture_settings, texture_version, pivot, shader, 0, fickle_type, visible)));
		std::shared_ptr<ActorTesselation2D> tessel(std::make_shared<ActorTesselation2D>(rect_render.get(), fickle_type));
		m_Fickler.Attach(tessel->Fickler());
		m_Map.push_back({ std::move(rect_render), std::move(tessel) });
	}
	m_Ordering = Permutation(m_Atlas->GetPlacements().size());
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

void TileMap::Insert(TileMapIndex tessel, float posX, float posY, const Modulate& modulate)
{
	if (m_Fickler.transformable)
	{
		if (m_Fickler.modulatable)
			m_Map[tessel].tessel->PushBackStatic({ {posX * m_Map[tessel].rectRender->GetUVWidth(), posY * m_Map[tessel].rectRender->GetUVHeight()} }, modulate );
		else
			m_Map[tessel].tessel->PushBackStatic({ {posX * m_Map[tessel].rectRender->GetUVWidth(), posY * m_Map[tessel].rectRender->GetUVHeight()} });
	}
}

ActorTesselation2D* const TileMap::TesselationRef(TileMapIndex i) const
{
	if (i >= 0 && i < m_Ordering.size())
		return m_Map[i].tessel.get();
	else return nullptr;
}
