#include "BatchSlotter.h"

BatchSlotter2D::BatchSlotter2D(ZIndex z)
	: ActorRenderBase2D(z)
{
}

BatchSlotter2D::BatchSlotter2D(const BatchSlotter2D& other)
	: m_Slots(other.m_Slots), ActorRenderBase2D(other)
{
}

BatchSlotter2D::BatchSlotter2D(BatchSlotter2D&& other) noexcept
	: m_Slots(std::move(other.m_Slots)), ActorRenderBase2D(std::move(other))
{
}

BatchSlotter2D& BatchSlotter2D::operator=(const BatchSlotter2D& other)
{
	m_Slots = other.m_Slots;
	ActorRenderBase2D::operator=(other);
	return *this;
}

BatchSlotter2D& BatchSlotter2D::operator=(BatchSlotter2D&& other) noexcept
{
	m_Slots = std::move(other.m_Slots);
	ActorRenderBase2D::operator=(std::move(other));
	return *this;
}

void BatchSlotter2D::RequestDraw(CanvasLayer* canvas_layer)
{
	for (auto iter = m_Slots.begin(); iter != m_Slots.end(); iter++)
		for (auto vit = iter->second.begin(); vit != iter->second.end(); vit++)
			(*vit)->RequestDraw(canvas_layer);
}

void BatchSlotter2D::Insert(const std::shared_ptr<ActorPrimitive2D>& primitive)
{
	auto iter = m_Slots.find(primitive->GetRenderable().model);
	if (iter != m_Slots.end())
		iter->second.push_back(primitive);
	else
		m_Slots.insert({ primitive->GetRenderable().model, { primitive } });
}

bool BatchSlotter2D::Remove(const std::shared_ptr<ActorPrimitive2D>& primitive)
{
	auto iter = m_Slots.find(primitive->GetRenderable().model);
	if (iter == m_Slots.end())
		return false;
	auto pos = std::find(iter->second.begin(), iter->second.end(), primitive);
	if (pos == iter->second.end())
		return false;
	iter->second.erase(pos);
	return true;
}
