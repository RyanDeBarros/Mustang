#pragma once

#include "render/ActorRenderBase.h"
#include "ActorPrimitive.h"

typedef unsigned short PrimitiveIndex;

class ActorSequencer2D : public ActorRenderBase2D
{
protected:
	Transformer2D m_Transformer;

public:
	inline ActorSequencer2D(ZIndex z = 0, const Transformer2D& transformer = {}) : ActorRenderBase2D(z), m_Transformer(transformer) {}
	inline ActorSequencer2D(const ActorSequencer2D& other) : ActorRenderBase2D(other), m_Transformer(other.m_Transformer) {}
	inline ActorSequencer2D(ActorSequencer2D&& other) noexcept : ActorRenderBase2D(std::move(other)), m_Transformer(std::move(other.m_Transformer)) {}
	inline ActorSequencer2D& operator=(const ActorSequencer2D& other)
	{
		ActorRenderBase2D::operator=(other);
		m_Transformer = other.m_Transformer;
		return *this;
	}
	inline ActorSequencer2D& operator=(ActorSequencer2D&& other) noexcept
	{
		ActorRenderBase2D::operator=(std::move(other));
		m_Transformer = std::move(other.m_Transformer);
		return *this;
	}

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
	
	virtual ActorPrimitive2D* const operator[](PrimitiveIndex i) { return nullptr; }
	virtual BufferCounter PrimitiveCount() const = 0;
	virtual void OnPreDraw() {}
	virtual void OnPostDraw() {}

	inline Transformer2D* Transformer() { return &m_Transformer; };
	inline Transform2D* Transform() { return &m_Transformer.self.transform; };
};
