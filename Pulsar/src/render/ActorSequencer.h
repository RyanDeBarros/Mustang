#pragma once

class ActorSequencer2D
{
public:
	virtual class ActorPrimitive2D* operator[](const int& i) const = 0;
};
