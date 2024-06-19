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
	ActorRenderBase2D_P m_Actor;
	std::vector<TesselRect> m_RectVector;

	void SetZIndex(const ZIndex& z) override;
public:
	ActorTesselation2D(ActorRenderBase2D_P actor);
	~ActorTesselation2D();

	inline ActorRenderBase2D_P ActorRef() const { return m_Actor; }
	inline std::vector<TesselRect>& RectVectorRef() { return m_RectVector; }

	void Insert(const TesselRect& rect);
	
	ActorPrimitive2D* operator[](const int& i) override;
	ZIndex GetZIndex() const override;
	BufferCounter PrimitiveCount() const override;
};
