#pragma once

#include <vector>

#include "Typedefs.h"
#include "ActorSequencer.h"
#include "ActorPrimitive.h"
#include "ActorTesselation.h"
#include "factory/Atlas.h"
#include "RectRender.h"

class TileMap : public ActorSequencer2D
{
	Atlas* m_Atlas;
	std::vector<std::pair<RectRender*, ActorTesselation2D*>> m_Map;

public:
	TileMap(const TileHandle& atlas_handle, const TextureSettings& texture_settings = Texture::nearest_settings, const ShaderHandle& shader = ShaderFactory::standard_shader, const ZIndex& z = 0, const bool& visible = true);
	~TileMap();

	ActorPrimitive2D* const operator[](const int& i) override;
	virtual BufferCounter PrimitiveCount() const;
	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	ActorTesselation2D* const TesselationRef(const int& i) const;
};
