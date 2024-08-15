#include "ActorComposite.h"

#include <string>
#include <stdexcept>

#include "render/CanvasLayer.h"

constexpr ActorPrimitiveCounter STATIC_INCR_AMOUNT = 1;
constexpr ActorPrimitiveCounter DYNAMIC_INCR_AMOUNT = 5;
constexpr ActorPrimitiveCounter VOLATILE_INCR_FACTOR = 2;

ActorComposite2D::ActorComposite2D(CompositeMode mode, ActorPrimitiveCounter initial_size, ZIndex z)
	: m_Mode(mode), ActorRenderBase2D(z)
{
	tail = new ActorRenderBase2D*[initial_size];
	head = tail;
	cap = tail + initial_size;
}

ActorComposite2D::ActorComposite2D(const ActorComposite2D& other)
	: m_Mode(other.m_Mode), ActorRenderBase2D(other)
{
	auto initial_size = other.cap - other.tail;
	tail = new ActorRenderBase2D*[initial_size];
	head = tail;
	cap = tail + initial_size;
}

ActorComposite2D::ActorComposite2D(ActorComposite2D&& other) noexcept
	: m_Mode(other.m_Mode), ActorRenderBase2D(std::move(other)), tail(other.tail), head(other.head), cap(other.cap)
{
	other.tail = nullptr;
}

ActorComposite2D& ActorComposite2D::operator=(const ActorComposite2D& other)
{
	ActorRenderBase2D::operator=(other);
	m_Mode = other.m_Mode;
	if (tail)
		delete[] tail;
	auto initial_size = other.cap - other.tail;
	tail = new ActorRenderBase2D*[initial_size];
	head = tail;
	cap = tail + initial_size;
	return *this;
}

ActorComposite2D& ActorComposite2D::operator=(ActorComposite2D&& other) noexcept
{
	ActorRenderBase2D::operator=(std::move(other));
	m_Mode = other.m_Mode;
	if (tail)
		delete[] tail;
	tail = other.tail;
	head = other.head;
	cap = other.cap;
	other.tail = nullptr;
	return *this;
}

ActorComposite2D::~ActorComposite2D()
{
	if (tail)
		delete[] tail;
	cap = head = tail = nullptr;
}

ActorRenderBase2D* const ActorComposite2D::operator[](PrimitiveIndex i)
{
	if (head - tail > i)
		return nullptr;
	return *(tail + i);
}

void ActorComposite2D::RequestDraw(CanvasLayer* canvas_layer)
{
	ActorRenderBase2D** iter = tail;
	while (iter != head)
	{
		(*iter)->RequestDraw(canvas_layer);
		iter++;
	}
}

void ActorComposite2D::Push(ActorRenderBase2D* primitive)
{
	_try_increase_alloc();
	*head = primitive;
	head++;
}

ActorRenderBase2D* ActorComposite2D::Get(ActorPrimitiveCounter index)
{
	_assert_valid_index(index);
	return *(tail + index);
}

ActorPrimitiveCounter ActorComposite2D::Index(ActorRenderBase2D* primitive)
{
	ActorRenderBase2D** current = tail;
	while (current != head)
	{
		if (*current == primitive)
			return current - tail;
		current++;
	}
	return -1;
}

void ActorComposite2D::Insert(ActorRenderBase2D* primitive, ActorPrimitiveCounter index)
{
	_assert_valid_index(index);
	_try_increase_alloc();
	ActorRenderBase2D** current = head - 1;
	while (current - tail >= index)
	{
		*(current + 1) = *current;
		current--;
	}
	*current = primitive;
	head++;
}

ActorRenderBase2D* ActorComposite2D::Replace(ActorRenderBase2D* primitive, ActorPrimitiveCounter index)
{
	_assert_valid_index(index);
	ActorRenderBase2D* temp = *(tail + index);
	*(tail + index) = primitive;
	return temp;
}

ActorRenderBase2D* ActorComposite2D::Remove(ActorPrimitiveCounter index)
{
	_assert_valid_index(index);
	ActorRenderBase2D** current = tail + index;
	ActorRenderBase2D* temp = *current;
	while (current - head < 0)
	{
		*current = *(current + 1);
		current++;
	}
	head--;
	_try_decrease_alloc();
	return temp;
}

bool ActorComposite2D::Erase(ActorRenderBase2D* primitive)
{
	ActorRenderBase2D** current = tail;
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
		ActorRenderBase2D** temp = new ActorRenderBase2D*[new_size];
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
		ActorRenderBase2D** temp = new ActorRenderBase2D*[new_size];
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
