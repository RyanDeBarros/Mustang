#include "DebugBatcher.h"

#include <unordered_set>

#include "render/CanvasLayer.h"

void DebugBatcher::RequestDraw(CanvasLayer* canvas_layer)
{
	for (auto& [pair, multi_polygon] : m_Slots)
	{
		canvas_layer->DrawMultiArray(multi_polygon);
	}
}

bool DebugBatcher::ChangeZIndex(const std::vector<std::shared_ptr<DebugPolygon>>::iterator& where, const ZIndex& z)
{
	DebugModel pair = { (*where)->m_IndexingMode, (*where)->m_Renderable.model };
	const auto& multi_polygon = m_Slots.find(pair);
	if (multi_polygon != m_Slots.end())
	{
		multi_polygon->second.ChangeZIndex(where, z);
		return true;
	}
	return false;
}

void DebugBatcher::PushBack(const std::shared_ptr<DebugPolygon>& poly)
{
	DebugModel pair = { poly->m_IndexingMode, poly->m_Renderable.model };
	const auto& multi_polygon = m_Slots.find(pair);
	if (multi_polygon != m_Slots.end())
		multi_polygon->second.PushBack(poly);
	else
	{
		DebugMultiPolygon multi(pair);
		multi.PushBack(poly);
		m_Slots.emplace(pair, std::move(multi));
	}
}

void DebugBatcher::PushBackAll(const std::vector<std::shared_ptr<DebugPolygon>>& polys)
{
	std::unordered_set<std::pair<GLenum, BatchModel>> pushed;
	DebugModel pair;
	for (const auto& poly : polys)
	{
		pair = { poly->m_IndexingMode, poly->m_Renderable.model };
		pushed.insert(pair);
		const auto& multi_polygon = m_Slots.find(pair);
		if (multi_polygon != m_Slots.end())
			multi_polygon->second.BufferPush(poly);
		else
		{
			DebugMultiPolygon multi(pair);
			multi.BufferPush(poly);
			m_Slots.emplace(pair, std::move(multi));
		}
	}
	for (auto iter = pushed.begin(); iter != pushed.end(); iter++)
		m_Slots[*iter].FlushPush();
}

bool DebugBatcher::Erase(const std::vector<std::shared_ptr<DebugPolygon>>::iterator& where)
{
	DebugModel pair = { (*where)->m_IndexingMode, (*where)->m_Renderable.model };
	const auto& multi_polygon = m_Slots.find(pair);
	if (multi_polygon != m_Slots.end())
	{
		multi_polygon->second.Erase(where);
		if (multi_polygon->second.draw_count == 0)
			m_Slots.erase(pair);
		return true;
	}
	return false;
}

bool DebugBatcher::Find(const std::shared_ptr<DebugPolygon>& poly, std::vector<std::shared_ptr<DebugPolygon>>::iterator& where)
{
	const auto& multi_polygon = m_Slots.find({ poly->m_IndexingMode, poly->m_Renderable.model });
	if (multi_polygon != m_Slots.end())
	{
		where = multi_polygon->second.Find(poly);
		return true;
	}
	return false;
}
