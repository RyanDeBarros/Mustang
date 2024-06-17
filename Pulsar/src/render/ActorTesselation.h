#pragma once

#include <vector>

#include "Typedefs.h"
#include "ActorPrimitive.h"
#include "ActorSequencer.h"

struct TesselRect
{
	float x, y, r, w, h;
};

class ActorTesselation2D : virtual public ActorSequencer2D
{
	ActorPrimitive2D m_Actor;
	std::vector<TesselRect> m_RectVector;

	inline void SetZIndex(const ZIndex& z) override { m_Actor.m_Z = z; }
public:
	ActorTesselation2D(ActorPrimitive2D actor);
	~ActorTesselation2D();

	inline ActorPrimitive2D& ActorRef() { return m_Actor; }
	inline std::vector<TesselRect>& RectVectorRef() { return m_RectVector; }

	void Insert(const TesselRect& rect);
	
	ActorPrimitive2D* operator[](const int& i) override;
	inline ZIndex GetZIndex() const override { return m_Actor.m_Z; }
};
