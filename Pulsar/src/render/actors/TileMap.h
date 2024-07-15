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

struct TMElement
{
	RectRender* rectRender;
	ActorTesselation2D* tessel;
};

class TileMap : public ActorSequencer2D
{
	Atlas* m_Atlas;
	std::vector<TMElement> m_Map;
	Permutation m_Ordering;
	Transform2D m_Transform;

public:
	TileMap(const TileHandle& atlas_handle, const TextureSettings& texture_settings = Texture::nearest_settings, const ShaderHandle& shader = ShaderFactory::standard_shader, const ZIndex& z = 0, const bool& visible = true);
	~TileMap();

	ActorPrimitive2D* const operator[](const int& i) override;
	virtual BufferCounter PrimitiveCount() const;
	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	bool SetOrdering(const Permutation& permutation);
	inline void SetTransform(const Transform2D& tr) { m_Transform = tr; }
	void Insert(const size_t& tessel, float posX, float posY);

	ActorTesselation2D* const TesselationRef(const size_t& tessel) const;
};
