#include "TileMap.h"

#include <algorithm>

TileMap::TileMap(TileHandle atlas_handle, const TextureSettings& texture_settings, ShaderHandle shader, ZIndex z, FickleType fickle_type, bool visible)
	: FickleActor2D(fickle_type, z)
{
	Tile* t = TileFactory::GetTileRef(atlas_handle);
	m_Atlas = dynamic_cast<Atlas*>(t);
	if (!m_Atlas)
		throw atlas_cast_error();
	for (TileMapIndex i = 0; i < m_Atlas->GetPlacements().size(); i++)
	{
		std::unique_ptr<RectRender> rect_render(std::make_unique<RectRender>(m_Atlas->SampleSubtile(i, texture_settings, shader, 0, fickle_type, visible)));
		std::shared_ptr<ActorTesselation2D> tessel(std::make_shared<ActorTesselation2D>(rect_render.get(), fickle_type));
		if (m_Fickler.IsTransformable()) [[likely]]
			m_Fickler.Transformer()->Attach(tessel->Fickler().Transformer());
		else if (m_Fickler.IsProtean())
			m_Fickler.ProteanLinker()->Attach(tessel->Fickler().ProteanLinker());
		else if (m_Fickler.IsModulatable()) [[unlikely]]
			m_Fickler.Modulator()->Attach(tessel->Fickler().Modulator());
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

void TileMap::Insert(TileMapIndex tessel, float posX, float posY, const Modulate& modulate)
{
	if (m_Fickler.IsTransformable())
		m_Map[tessel].tessel->PushBackStatic({ {posX * m_Map[tessel].rectRender->GetUVWidth(), posY * m_Map[tessel].rectRender->GetUVHeight()} });
	if (m_Fickler.IsProtean())
		m_Map[tessel].tessel->PushBackStatic({ { {posX * m_Map[tessel].rectRender->GetUVWidth(), posY * m_Map[tessel].rectRender->GetUVHeight()} }, modulate });
}

ActorTesselation2D* const TileMap::TesselationRef(TileMapIndex i) const
{
	if (i >= 0 && i < m_Ordering.size())
		return m_Map[i].tessel.get();
	else return nullptr;
}
