#pragma once

#include <map>

#include "Typedefs.h"
#include "ActorSequencer.h"
#include "ActorPrimitive.h"
#include "ActorTesselation.h"
#include "factory/Atlas.h"

class TileMap : public ActorSequencer2D
{
	mutable int cache_i = 0;
	mutable size_t cache_iter_offset = 0;
	mutable std::map<TileHandle, ActorTesselation2D>::iterator cache_iter;
	Atlas* m_Atlas;
	std::map<TileHandle, ActorTesselation2D> m_Map;

public:
	TileMap(const int& atlas_width, const int& atlas_height);
	TileMap(Atlas* atlas);
	~TileMap();

	ActorPrimitive2D* operator[](const int& i) override;

private:
	void ClearCache();
};
