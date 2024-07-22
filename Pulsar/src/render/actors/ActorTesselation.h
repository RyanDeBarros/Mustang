#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "Typedefs.h"
#include "ActorPrimitive.h"
#include "ActorSequencer.h"

class ActorTesselation2D : virtual public ActorSequencer2D, public Transformable2D
{
private:
	std::shared_ptr<ActorRenderBase2D> m_Actor;
	MultiLocalTransformer2D m_Transformer;
	std::vector<Transform2D> m_ActorPreDrawTransforms;

	std::function<ActorPrimitive2D* const(const int& i)> f_operator;
	std::function<BufferCounter()> f_RenderSeqCount;
	std::function<void()> f_OnPreDraw;
	std::function<void()> f_OnPostDraw;

	static ActorPrimitive2D* const f_prim_operator(ActorTesselation2D* const, const int&);
	static ActorPrimitive2D* const f_sequ_operator(ActorTesselation2D* const, const int&);
	static BufferCounter f_prim_RenderSeqCount(ActorTesselation2D const* const);
	static BufferCounter f_sequ_RenderSeqCount(ActorTesselation2D const* const);
	static void f_prim_OnPreDraw(ActorTesselation2D * const);
	static void f_sequ_OnPreDraw(ActorTesselation2D * const);
	static void f_prim_OnPostDraw(ActorTesselation2D * const);
	static void f_sequ_OnPostDraw(ActorTesselation2D * const);

	void BindFunctions();
	BufferCounter RenderSeqCount() const;

public:
	ActorTesselation2D(const std::shared_ptr<ActorRenderBase2D>& actor);
	ActorTesselation2D(const ActorTesselation2D&);
	ActorTesselation2D(ActorTesselation2D&&) noexcept;
	~ActorTesselation2D();

	inline std::shared_ptr<ActorRenderBase2D> ActorRef() const { return m_Actor; }
	inline MultiLocalTransformer2D* const TransformerRef() { return &m_Transformer; }
	void PushBack(const std::shared_ptr<Transformable2D>& child);
	void PushBack(const std::vector<std::shared_ptr<Transformable2D>>& children);
	
	ActorPrimitive2D* const operator[](const int& i) override;
	ZIndex GetZIndex() const override;
	void SetZIndex(const ZIndex& z) override;
	BufferCounter PrimitiveCount() const override;
	void OnPreDraw() override;
	void OnPostDraw() override;
};
