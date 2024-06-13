#pragma once

#include "Typedefs.h"
#include "ActorPrimitive.h"

enum class CompositeMode
{
	STATIC,
	DYNAMIC,
	VOLATILE
};

class ActorComposite2D
{
	friend class ActorRenderIterator;
	friend class CanvasLayer;
	CompositeMode m_Mode;
	ActorPrimitive2D** tail;
	ActorPrimitive2D** head;
	ActorPrimitive2D** cap;
	ZIndex m_CompositeZ;
public:
	ActorComposite2D(CompositeMode mode, ActorPrimitiveCounter initial_size = 1, ZIndex z = 0);
	~ActorComposite2D();

	void Push(ActorPrimitive2D* primitive);
	ActorPrimitive2D* Get(ActorPrimitiveCounter index);
	ActorPrimitiveCounter Index(ActorPrimitive2D* primitive);
	void Insert(ActorPrimitive2D* primitive, ActorPrimitiveCounter index);
	ActorPrimitive2D* Replace(ActorPrimitive2D* primitive, ActorPrimitiveCounter index);
	ActorPrimitive2D* Remove(ActorPrimitiveCounter index);
	bool Erase(ActorPrimitive2D* primitive);

	inline ZIndex GetZIndex() const { return m_CompositeZ; }
private:
	void _assert_valid_index(ActorPrimitiveCounter index);
	void _try_increase_alloc();
	void _try_decrease_alloc();
	bool _decrease_condition();
};