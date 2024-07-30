#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "Typedefs.h"
#include "ActorPrimitive.h"
#include "ActorSequencer.h"
#include "../transform/MultiTransformer.h"

class ActorTesselation2D : virtual public ActorSequencer2D
{
private:
	std::shared_ptr<ActorRenderBase2D> m_Actor;
	std::shared_ptr<TransformableProxy2D> m_Transform;
	MultiTransformer2D m_Transformer;
	std::vector<Transform2D> m_ActorPreDrawTransforms;

	std::vector<std::shared_ptr<TransformableProxy2D>> m_TransformsList;

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
	ActorTesselation2D(const std::shared_ptr<ActorRenderBase2D>& actor);
	ActorTesselation2D(std::shared_ptr<ActorRenderBase2D>&& actor);
	ActorTesselation2D(const ActorTesselation2D&);
	ActorTesselation2D(ActorTesselation2D&&) noexcept;
	ActorTesselation2D& operator=(const ActorTesselation2D&);
	ActorTesselation2D& operator=(ActorTesselation2D&&) noexcept;
	~ActorTesselation2D();

	inline std::shared_ptr<ActorRenderBase2D> ActorRef() const { return m_Actor; }
	inline std::shared_ptr<TransformableProxy2D> Transform() { return m_Transform; }
	inline MultiTransformer2D* const Transformer() { return &m_Transformer; }
	void PushBackGlobal(const Transform2D& child);
	void PushBackGlobals(const std::vector<Transform2D>& children);
	void PushBackLocal(const Transform2D& local);
	void PushBackLocals(const std::vector<Transform2D>& locals);
	
	ActorPrimitive2D* const operator[](PrimitiveIndex i) override;
	ZIndex GetZIndex() const override;
	void SetZIndex(ZIndex z) override;
	BufferCounter PrimitiveCount() const override;
	void OnPreDraw() override;
	void OnPostDraw() override;

	template<std::derived_from<ActorRenderBase2D> ActorType>
	inline static std::shared_ptr<ActorTesselation2D> MakeShared(const ActorType& actor)
	{
		return std::make_shared<ActorTesselation2D>(std::make_shared<ActorType>(actor));
	}
};
