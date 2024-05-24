#pragma once

#include "render/ActorPrimitive.h"
#include "render/ActorComposite.h"

class ActorRenderIterator
{
	ActorPrimitive2D** tail;
	ActorPrimitive2D** head;
	ActorPrimitive2D** current;

public:
	ActorRenderIterator(ActorPrimitive2D* primitive)
		: tail(&primitive), head(&primitive), current(tail)
	{}
	ActorRenderIterator(ActorComposite2D* composite)
		: tail(composite->tail), head(composite->head - 1), current(tail)
	{}

	void operator++()
	{
		if (current)
		{
			current++;
			if (current - head > 0)
				current = nullptr;
		}
	}

	void operator--()
	{
		if (current)
		{
			current--;
			if (tail - current > 0)
				current = nullptr;
		}
	}

	ActorPrimitive2D* operator*()
	{
		if (head - tail < 0)
			return nullptr;
		return *current;
	}
};
