#pragma once

#include <glm/glm.hpp>

#include <vector>

#include "Typedefs.h"
#include "Utils.h"
#include "ActorSequencer.h"
#include "ActorPrimitive.h"
#include "ActorTesselation.h"
#include "factory/Atlas.h"
#include "RectRender.h"
#include "../transform/Transformer.h"

struct TMElement
{
	std::shared_ptr<RectRender> rectRender;
	std::shared_ptr<ActorTesselation2D> tessel;
	Transformer2D transformer;
};

class TileMap : public ActorRenderBase2D
{
	Atlas* m_Atlas;
	std::vector<TMElement> m_Map;
	Permutation m_Ordering;
	std::shared_ptr<Transform2D> m_Transform;

public:
	TileMap(const TileHandle& atlas_handle, const TextureSettings& texture_settings = Texture::nearest_settings, const ShaderHandle& shader = ShaderFactory::standard_shader, const ZIndex& z = 0, const bool& visible = true);
	TileMap(const TileMap&) = delete;
	TileMap(TileMap&&) = delete;

	virtual BufferCounter PrimitiveCount() const;
	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	bool SetOrdering(const Permutation& permutation);
	void SetTransform(const Transform2D& tr);
	void Insert(const size_t& tessel, float posX, float posY);

	ActorTesselation2D* const TesselationRef(const size_t& tessel) const;
};
