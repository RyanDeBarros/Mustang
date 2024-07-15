#include "DebugBatcher.h"

#include <unordered_set>

#include "render/CanvasLayer.h"

void DebugBatcher::RequestDraw(CanvasLayer* canvas_layer)
{
	for (auto multi_polygon : m_OrderedTraversal)
	{
		multi_polygon->RequestDraw(canvas_layer);
	}
}

bool DebugBatcher::ChangeZIndex(const DebugModel& model, const ZIndex& z)
{
	auto multi_polygon = m_Slots.find(model);
	if (multi_polygon == m_Slots.end())
		return false;
	multi_polygon->second.SetZIndex(z);
	Sort();
	return true;
}

bool DebugBatcher::ChangeZIndex(const DebugMultiPolygon::iterator& where, const ZIndex& z)
{
	const auto& multi_polygon = m_Slots.find((*where)->GetDebugModel());
	if (multi_polygon != m_Slots.end())
	{
		multi_polygon->second.ChangeZIndex(where, z);
		return true;
	}
	return false;
}

void DebugBatcher::PushBack(const std::shared_ptr<DebugPolygon>& poly)
{
	DebugModel pair = poly->GetDebugModel();
	const auto& multi_polygon = m_Slots.find(pair);
	if (multi_polygon != m_Slots.end())
		multi_polygon->second.PushBack(poly);
	else
	{
		DebugMultiPolygon multi(pair);
		multi.PushBack(poly);
		m_Slots.emplace(pair, std::move(multi));
		Sort();
	}
}

void DebugBatcher::PushBackAll(const std::vector<std::shared_ptr<DebugPolygon>>& polys)
{
	std::unordered_set<std::pair<GLenum, BatchModel>> pushed;
	DebugModel pair;
	for (const auto& poly : polys)
	{
		pair = poly->GetDebugModel();
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
	Sort();
}

bool DebugBatcher::Erase(const std::vector<std::shared_ptr<DebugPolygon>>::iterator& where)
{
	DebugModel pair = (*where)->GetDebugModel();
	const auto& multi_polygon = m_Slots.find(pair);
	if (multi_polygon != m_Slots.end())
	{
		multi_polygon->second.Erase(where);
		if (multi_polygon->second.draw_count == 0)
		{
			m_Slots.erase(pair);
			Sort();
		}
		return true;
	}
	return false;
}

bool DebugBatcher::Find(const std::shared_ptr<DebugPolygon>& poly, DebugMultiPolygon::iterator& where)
{
	const auto& multi_polygon = m_Slots.find(poly->GetDebugModel());
	if (multi_polygon != m_Slots.end())
	{
		where = multi_polygon->second.Find(poly);
		return true;
	}
	return false;
}

void DebugBatcher::Sort()
{
	for (auto& pair : m_Slots)
		m_OrderedTraversal.push_back(&pair.second);
	std::sort(m_OrderedTraversal.begin(), m_OrderedTraversal.end(),
		[](const DebugMultiPolygon* const first, const DebugMultiPolygon* const second)
		{
			return first->GetZIndex() < second->GetZIndex();
		});
}