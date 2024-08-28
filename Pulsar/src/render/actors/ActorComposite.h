#pragma once

#include "../ActorRenderBase.h"

enum class CompositeMode
{
	STATIC,
	DYNAMIC,
	VOLATILE
};

typedef unsigned short PrimitiveIndex;

class ActorComposite2D : public FickleActor2D
{
	friend class CanvasLayer;
	CompositeMode m_Mode;
	ActorRenderBase2D** tail;
	ActorRenderBase2D** head;
	ActorRenderBase2D** cap;

public:
	ActorComposite2D(CompositeMode mode, size_t initial_size = 1, ZIndex z = 0, FickleType fickle_type = FickleType::Protean);
	ActorComposite2D(const ActorComposite2D&);
	ActorComposite2D(ActorComposite2D&&) noexcept;
	ActorComposite2D& operator=(const ActorComposite2D&);
	ActorComposite2D& operator=(ActorComposite2D&&) noexcept;
	~ActorComposite2D();

	ActorRenderBase2D* const operator[](PrimitiveIndex i);
	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	void Push(ActorRenderBase2D* primitive);
	ActorRenderBase2D* Get(size_t index);
	size_t Index(ActorRenderBase2D* primitive);
	void Insert(ActorRenderBase2D* primitive, size_t index);
	ActorRenderBase2D* Replace(ActorRenderBase2D* primitive, size_t index);
	ActorRenderBase2D* Remove(size_t index);
	bool Erase(ActorRenderBase2D* primitive);

private:
	void _assert_valid_index(size_t index);
	void _try_increase_alloc();
	void _try_decrease_alloc();
	bool _decrease_condition();
};
