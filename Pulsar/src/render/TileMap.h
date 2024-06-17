#pragma once

#include <unordered_map>
#include <unordered_set>

#include "ActorSequencer.h"
#include "ActorPrimitive.h"
#include "factory/Atlas.h"
#include "factory/Tile.h"

class TileMap : public ActorSequencer2D
{
	Atlas* m_Atlas;
	std::unordered_map<TileHandle, std::unordered_set<TileRect>> m_Map;

public:
	TileMap(const int& atlas_width, const int& atlas_height);
	TileMap(Atlas* atlas);
	~TileMap();

	ActorPrimitive2D* operator[](const int& i) const override;
};
