#pragma once

#include <vector>
#include <functional>

#include "Typedefs.h"
#include "ActorPrimitive.h"
#include "ActorSequencer.h"

class ActorTesselation2D : virtual public ActorSequencer2D
{
private:
	ActorRenderBase2D* const m_Actor;
	std::vector<Transform2D> m_RectVector;
	std::vector<Transform2D> m_ActorOffsets;
	Transform2D m_GlobalTransform;

	std::function<ActorPrimitive2D* const(const int& i)> f_operator;
	std::function<BufferCounter()> f_RenderSeqCount;
	std::function<void()> f_OnPreDraw;
	std::function<void()> f_OnPostDraw;

	static ActorPrimitive2D* const f_prim_operator(ActorTesselation2D const* const, const int&);
	static ActorPrimitive2D* const f_sequ_operator(ActorTesselation2D const* const, const int&);
	static BufferCounter f_prim_RenderSeqCount(ActorTesselation2D const* const);
	static BufferCounter f_sequ_RenderSeqCount(ActorTesselation2D const* const);
	static void f_prim_OnPreDraw(ActorTesselation2D * const);
	static void f_sequ_OnPreDraw(ActorTesselation2D * const);
	static void f_prim_OnPostDraw(ActorTesselation2D * const);
	static void f_sequ_OnPostDraw(ActorTesselation2D * const);

	BufferCounter RenderSeqCount() const;
	void SetZIndex(const ZIndex& z) override;
public:
	ActorTesselation2D(ActorRenderBase2D* const actor);
	~ActorTesselation2D();

	inline ActorRenderBase2D* const ActorRef() const { return m_Actor; }
	inline std::vector<Transform2D>& RectVectorRef() { return m_RectVector; }

	void Insert(const Transform2D& rect);
	inline void SetPosition(const float& x, const float& y) { m_GlobalTransform.position.x = x; m_GlobalTransform.position.y = y; }
	inline void SetRotation(const float& r) { m_GlobalTransform.rotation = r; }
	inline void SetScale(const float& x, const float& y) { m_GlobalTransform.scale.x = x; m_GlobalTransform.scale.y = y; }
	
	ActorPrimitive2D* const operator[](const int& i) override;
	ZIndex GetZIndex() const override;
	BufferCounter PrimitiveCount() const override;
	void OnPreDraw() override;
	void OnPostDraw() override;
};
