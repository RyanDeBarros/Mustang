#include "ActorComposite.h"

#include <string>
#include <stdexcept>

constexpr ActorPrimitiveCounter STATIC_INCR_AMOUNT = 1;
constexpr ActorPrimitiveCounter DYNAMIC_INCR_AMOUNT = 5;
constexpr ActorPrimitiveCounter VOLATILE_INCR_FACTOR = 2;

ActorComposite2D::ActorComposite2D(CompositeMode mode, ActorPrimitiveCounter initial_size, ZIndex z)
	: m_Mode(mode), ActorSequencer2D(z)
{
	tail = new ActorPrimitive2D*[initial_size];
	head = tail;
	cap = tail + initial_size;
}

ActorComposite2D::~ActorComposite2D()
{
	if (tail)
		delete[] tail;
	cap = head = tail = nullptr;
}

ActorPrimitive2D* const ActorComposite2D::operator[](const int& i)
{
	if (head - tail > i)
		return nullptr;
	return *(tail + i);
}

void ActorComposite2D::Push(ActorPrimitive2D* primitive)
{
	_try_increase_alloc();
	*head = primitive;
	head++;
}

ActorPrimitive2D* ActorComposite2D::Get(ActorPrimitiveCounter index)
{
	_assert_valid_index(index);
	return *(tail + index);
}

ActorPrimitiveCounter ActorComposite2D::Index(ActorPrimitive2D* primitive)
{
	ActorPrimitive2D** current = tail;
	while (current != head)
	{
		if (*current == primitive)
			return current - tail;
		current++;
	}
	return -1;
}

void ActorComposite2D::Insert(ActorPrimitive2D* primitive, ActorPrimitiveCounter index)
{
	_assert_valid_index(index);
	_try_increase_alloc();
	ActorPrimitive2D** current = head - 1;
	while (current - tail >= index)
	{
		*(current + 1) = *current;
		current--;
	}
	*current = primitive;
	head++;
}

ActorPrimitive2D* ActorComposite2D::Replace(ActorPrimitive2D* primitive, ActorPrimitiveCounter index)
{
	_assert_valid_index(index);
	ActorPrimitive2D* temp = *(tail + index);
	*(tail + index) = primitive;
	return temp;
}

ActorPrimitive2D* ActorComposite2D::Remove(ActorPrimitiveCounter index)
{
	_assert_valid_index(index);
	ActorPrimitive2D** current = tail + index;
	ActorPrimitive2D* temp = *current;
	while (current - head < 0)
	{
		*current = *(current + 1);
		current++;
	}
	head--;
	_try_decrease_alloc();
	return temp;
}

bool ActorComposite2D::Erase(ActorPrimitive2D* primitive)
{
	ActorPrimitive2D** current = tail;
	while (current - head < 0)
	{
		if (*current == primitive)
		{
			while (current - head < 0)
			{
				*current = *(current + 1);
				current++;
			}
			head--;
			_try_decrease_alloc();
			return true;
		}
		current++;
	}
	return false;
}

void ActorComposite2D::_assert_valid_index(ActorPrimitiveCounter index)
{
	if (index < 0 || index >= head - tail)
		throw std::out_of_range(std::string("Index ") + std::to_string(index) + " out of range.");
}

void ActorComposite2D::_try_increase_alloc()
{
	if (head - cap >= 0)
	{
		ActorPrimitiveCounter head_len = head - tail;
		ActorPrimitiveCounter cap_len = cap - tail;
		ActorPrimitiveCounter new_size = cap_len;
		switch (m_Mode)
		{
		case CompositeMode::STATIC:
			new_size += STATIC_INCR_AMOUNT;
			break;
		case CompositeMode::DYNAMIC:
			new_size += DYNAMIC_INCR_AMOUNT;
			break;
		case CompositeMode::VOLATILE:
			new_size *= VOLATILE_INCR_FACTOR;
			break;
		}
		ActorPrimitive2D** temp = new ActorPrimitive2D*[new_size];
		memcpy_s(temp, new_size, tail, cap_len);
		delete[] tail;
		tail = temp;
		head = tail + head_len;
		cap = tail + new_size;
	}
}

void ActorComposite2D::_try_decrease_alloc()
{
	if (_decrease_condition())
	{
		ActorPrimitiveCounter head_len = head - tail;
		ActorPrimitiveCounter cap_len = cap - tail;
		ActorPrimitiveCounter new_size = cap_len;
		switch (m_Mode)
		{
		case CompositeMode::STATIC:
			new_size -= STATIC_INCR_AMOUNT;
			break;
		case CompositeMode::DYNAMIC:
			new_size -= DYNAMIC_INCR_AMOUNT;
			break;
		case CompositeMode::VOLATILE:
			new_size /= VOLATILE_INCR_FACTOR;
			break;
		}
		ActorPrimitive2D** temp = new ActorPrimitive2D*[new_size];
		memcpy_s(temp, new_size, tail, cap_len);
		delete[] tail;
		tail = temp;
		head = tail + head_len;
		cap = tail + new_size;
	}
}

bool ActorComposite2D::_decrease_condition()
{
	switch (m_Mode)
	{
	case CompositeMode::STATIC:
		return cap - head > 2 * STATIC_INCR_AMOUNT;
	case CompositeMode::DYNAMIC:
		return cap - head > 2 * DYNAMIC_INCR_AMOUNT;
	case CompositeMode::VOLATILE:
		return 4 * (head - tail) < (cap - tail);
	}
	return false;
}
