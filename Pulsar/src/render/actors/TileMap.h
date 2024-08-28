#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "utils/Permutation.h"
#include "ActorPrimitive.h"
#include "ActorTesselation.h"
#include "factory/Atlas.h"
#include "RectRender.h"
#include "../transform/Transform.h"

struct TMElement
{
	std::unique_ptr<RectRender> rectRender;
	std::shared_ptr<ActorTesselation2D> tessel;
};

typedef size_t TileMapIndex;

class TileMap : public FickleActor2D
{
	std::shared_ptr<const Atlas> m_Atlas;
	std::vector<TMElement> m_Map;
	Permutation m_Ordering;

public:
	TileMap(const std::shared_ptr<const Atlas>& atlas, const TextureSettings& texture_settings = Texture::nearest_settings, TextureVersion texture_version = 0, ShaderHandle shader = ShaderRegistry::standard_shader, ZIndex z = 0, FickleType fickle_type = FickleType::Transformable, bool visible = true);
	TileMap(const TileMap&) = delete;
	TileMap(TileMap&&) = delete;
	~TileMap();

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	bool SetOrdering(const Permutation& permutation);
	void Insert(TileMapIndex tessel, float posX, float posY, const Modulate& modulate = { 1.0f, 1.0f, 1.0f, 1.0f });

	ActorTesselation2D* const TesselationRef(TileMapIndex tessel) const;
};
