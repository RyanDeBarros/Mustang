#pragma once

#include "Typedefs.h"
#include "Logger.h"

class ActorRenderBase2D
{
public:
	static constexpr unsigned int static_classID = 0;
	virtual constexpr unsigned int classID() const { return ActorRenderBase2D::static_classID; }

	virtual ZIndex GetZIndex() const = 0;
	virtual void SetZIndex(const ZIndex& z) = 0;
};

#define ACTOR_RENDER_INHERIT(class_name, superclass)\
public:\
	static constexpr unsigned int static_classID = (1 << __COUNTER__) | superclass::static_classID;\
	virtual constexpr unsigned int classID() const override { return class_name::static_classID; }

#define INHERITS(actor, class_name) (actor).classID() & class_name::static_classID
