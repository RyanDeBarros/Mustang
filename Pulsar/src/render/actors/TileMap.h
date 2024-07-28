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
#include "../transform/MultiTransformer.h"

struct TMElement
{
	std::shared_ptr<RectRender> rectRender;
	std::shared_ptr<ActorTesselation2D> tessel;
};

class TileMap : public ActorRenderBase2D, public std::enable_shared_from_this<TileMap>
{
	Atlas* m_Atlas;
	std::vector<TMElement> m_Map;
	Permutation m_Ordering;
	std::shared_ptr<TransformableProxy2D> m_Transform;
	MultiTransformer2D m_Transformer;

public:
	TileMap(TileHandle atlas_handle, const TextureSettings& texture_settings = Texture::nearest_settings, ShaderHandle shader = ShaderFactory::standard_shader, ZIndex z = 0, bool visible = true);
	TileMap(const TileMap&) = delete;
	TileMap(TileMap&&) = delete;

	virtual BufferCounter PrimitiveCount() const;
	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	bool SetOrdering(const Permutation& permutation);
	void Insert(size_t tessel, float posX, float posY);

	// TODO use typedef instead of size_t

	ActorTesselation2D* const TesselationRef(size_t tessel) const;
	MultiTransformer2D* TransformerRef() { return &m_Transformer; }
};
