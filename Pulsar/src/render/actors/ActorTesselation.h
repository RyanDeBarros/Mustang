#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "Typedefs.h"
#include "ActorPrimitive.h"
#include "ActorSequencer.h"
#include "../transform/Transform.h"

// TODO add modulator implementation

class ActorTesselation2D : public ActorSequencer2D
{
	ActorRenderBase2D* m_Actor;
	std::vector<std::shared_ptr<Transformer2D>> m_InternalChildren;
	
	std::function<ActorPrimitive2D* const(PrimitiveIndex i)> f_operator;
	std::function<BufferCounter()> f_RenderSeqCount;
	std::function<void()> f_OnPreDraw;
	std::function<void()> f_OnPostDraw;

	static ActorPrimitive2D* const f_prim_operator(ActorTesselation2D* const, PrimitiveIndex);
	static ActorPrimitive2D* const f_sequ_operator(ActorTesselation2D* const, PrimitiveIndex);
	static BufferCounter f_prim_RenderSeqCount(ActorTesselation2D const* const);
	static BufferCounter f_sequ_RenderSeqCount(ActorTesselation2D const* const);
	static void f_prim_OnPreDraw(ActorTesselation2D * const);
	static void f_sequ_OnPreDraw(ActorTesselation2D * const);
	static void f_prim_OnPostDraw(ActorTesselation2D * const);
	static void f_sequ_OnPostDraw(ActorTesselation2D * const);

	void BindFunctions();
	BufferCounter RenderSeqCount() const;

public:
	ActorTesselation2D(ActorRenderBase2D* actor);
	ActorTesselation2D(const ActorTesselation2D&);
	ActorTesselation2D(ActorTesselation2D&&) noexcept;
	ActorTesselation2D& operator=(const ActorTesselation2D&);
	ActorTesselation2D& operator=(ActorTesselation2D&&) noexcept;
	~ActorTesselation2D();

	inline ActorRenderBase2D* ActorRef() const { return m_Actor; }
	void PushBackStatic(const Transform2D& transform, bool sync_on_attach = false);
	void PushBackStatic(const std::vector<Transform2D>& transforms, bool sync_on_attach = false);

	ActorPrimitive2D* const operator[](PrimitiveIndex i) override;
	ZIndex GetZIndex() const override;
	void SetZIndex(ZIndex z) override;
	BufferCounter PrimitiveCount() const override;
	void OnPreDraw() override;
	void OnPostDraw() override;
};
