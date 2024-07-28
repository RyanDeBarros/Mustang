#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "Typedefs.h"
#include "ActorPrimitive.h"
#include "ActorSequencer.h"
#include "../transform/MultiTransformer.h"

class ActorTesselation2D : virtual public ActorSequencer2D, public std::enable_shared_from_this<ActorTesselation2D>
{
private:
	std::shared_ptr<ActorRenderBase2D> m_Actor;
	std::shared_ptr<TransformableProxy2D> m_Transform;
	MultiTransformer2D m_Transformer;
	std::vector<Transform2D> m_ActorPreDrawTransforms;

	std::function<ActorPrimitive2D* const(int i)> f_operator;
	std::function<BufferCounter()> f_RenderSeqCount;
	std::function<void()> f_OnPreDraw;
	std::function<void()> f_OnPostDraw;

	static ActorPrimitive2D* const f_prim_operator(ActorTesselation2D* const, int);
	static ActorPrimitive2D* const f_sequ_operator(ActorTesselation2D* const, int);
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
	ActorTesselation2D(std::shared_ptr<ActorRenderBase2D>&& actor);
	ActorTesselation2D(const ActorTesselation2D&) = delete;
	ActorTesselation2D(ActorTesselation2D&&) noexcept;
	ActorTesselation2D& operator=(const ActorTesselation2D&) = delete;
	ActorTesselation2D& operator=(ActorTesselation2D&&) noexcept;
	~ActorTesselation2D();

	inline std::shared_ptr<ActorRenderBase2D> ActorRef() const { return m_Actor; }
	inline std::shared_ptr<TransformableProxy2D> TransformRef() { return m_Transform; }
	inline MultiTransformer2D* const TransformerRef() { return &m_Transformer; }
	inline void PushBackGlobal(const std::shared_ptr<Transformable2D>& child) { m_Transformer.PushBackGlobal(child, false); }
	inline void PushBackGlobal(std::shared_ptr<Transformable2D>&& child) { m_Transformer.PushBackGlobal(std::move(child), false); }
	inline void PushBackGlobals(const std::vector<std::shared_ptr<Transformable2D>>& children) { m_Transformer.PushBackGlobals(children, false); }
	inline void PushBackGlobals(std::vector<std::shared_ptr<Transformable2D>>&& children) { m_Transformer.PushBackGlobals(std::move(children), false); }
	inline void PushBackLocal(const Transform2D& local) { m_Transformer.PushBackLocal(local, false); }
	inline void PushBackLocals(const std::vector<Transform2D>& locals) { m_Transformer.PushBackLocals(locals, false); }
	
	ActorPrimitive2D* const operator[](int i) override;
	ZIndex GetZIndex() const override;
	void SetZIndex(ZIndex z) override;
	BufferCounter PrimitiveCount() const override;
	void OnPreDraw() override;
	void OnPostDraw() override;
};
