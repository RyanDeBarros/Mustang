#include "BatchSlotter.h"

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
