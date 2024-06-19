#pragma once

#include <vector>

#include "Typedefs.h"
#include "ActorPrimitive.h"
#include "ActorSequencer.h"

class ActorTesselation2D : virtual public ActorSequencer2D
{
	ActorRenderBase2D_P m_Actor;
	std::vector<Transform2D> m_RectVector;
	std::vector<Transform2D> m_ActorOffsets;
	Transform2D m_GlobalTransform;

	inline BufferCounter RenderSeqCount() const { return std::get<ActorSequencer2D* const>(m_Actor)->PrimitiveCount(); }

	void SetZIndex(const ZIndex& z) override;
public:
	ActorTesselation2D(ActorRenderBase2D_P actor);
	~ActorTesselation2D();

	inline ActorRenderBase2D_P ActorRef() const { return m_Actor; }
	inline std::vector<Transform2D>& RectVectorRef() { return m_RectVector; }

	void Insert(const Transform2D& rect);
	inline void SetPosition(const float& x, const float& y) { m_GlobalTransform.position.x = x; m_GlobalTransform.position.y = y; }
	inline void SetRotation(const float& r) { m_GlobalTransform.rotation = r; }
	inline void SetScale(const float& x, const float& y) { m_GlobalTransform.scale.x = x; m_GlobalTransform.scale.y = y; }
	
	ActorPrimitive2D* operator[](const int& i) override;
	ZIndex GetZIndex() const override;
	BufferCounter PrimitiveCount() const override;
	void OnPreDraw() override;
	void OnPostDraw() override;
};
