#include "DebugBatcher.h"

#include <unordered_set>

#include "render/CanvasLayer.h"

DebugBatcher::DebugBatcher(const DebugBatcher& other)
	: ActorRenderBase2D(other), m_Slots(other.m_Slots), m_OrderedTraversal(other.m_OrderedTraversal)
{
}

DebugBatcher::DebugBatcher(DebugBatcher&& other) noexcept
	: ActorRenderBase2D(std::move(other)), m_Slots(std::move(other.m_Slots)), m_OrderedTraversal(std::move(other.m_OrderedTraversal))
{
}

DebugBatcher& DebugBatcher::operator=(const DebugBatcher& other)
{
	m_Slots = other.m_Slots;
	m_OrderedTraversal = other.m_OrderedTraversal;
	ActorRenderBase2D::operator=(other);
	return *this;
}

DebugBatcher& DebugBatcher::operator=(DebugBatcher&& other) noexcept
{
	m_Slots = std::move(other.m_Slots);
	m_OrderedTraversal = std::move(other.m_OrderedTraversal);
	ActorRenderBase2D::operator=(std::move(other));
	return *this;
}

void DebugBatcher::RequestDraw(CanvasLayer* canvas_layer)
{
	for (auto multi_polygon : m_OrderedTraversal)
		multi_polygon->RequestDraw(canvas_layer);
}

bool DebugBatcher::ChangeZIndex(const DebugModel& model, ZIndex z)
{
	auto multi_polygon = m_Slots.find(model);
	if (multi_polygon == m_Slots.end())
		return false;
	multi_polygon->second.SetZIndex(z);
	Sort();
	return true;
}

bool DebugBatcher::ChangeZIndex(const DebugMultiPolygon::iterator& where, ZIndex z)
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
	DebugModel model = poly->GetDebugModel();
	const auto& multi_polygon = m_Slots.find(model);
	if (multi_polygon != m_Slots.end())
		multi_polygon->second.PushBack(poly);
	else
	{
		DebugMultiPolygon multi(model);
		multi.PushBack(poly);
		m_Slots.emplace(model, std::move(multi));
		Sort();
	}
}

void DebugBatcher::PushBack(std::shared_ptr<DebugPolygon>&& poly)
{
	DebugModel model = poly->GetDebugModel();
	const auto& multi_polygon = m_Slots.find(model);
	if (multi_polygon != m_Slots.end())
		multi_polygon->second.PushBack(std::move(poly));
	else
	{
		DebugMultiPolygon multi(model);
		multi.PushBack(std::move(poly));
		m_Slots.emplace(model, std::move(multi));
		Sort();
	}
}

void DebugBatcher::PushBackAll(const std::vector<std::shared_ptr<DebugPolygon>>& polys)
{
	std::unordered_set<std::pair<GLenum, BatchModel>> pushed;
	DebugModel model;
	for (const auto& poly : polys)
	{
		model = poly->GetDebugModel();
		pushed.insert(model);
		const auto& multi_polygon = m_Slots.find(model);
		if (multi_polygon != m_Slots.end())
			multi_polygon->second.BufferPush(poly);
		else
		{
			DebugMultiPolygon multi(model);
			multi.BufferPush(poly);
			m_Slots.emplace(model, std::move(multi));
		}
	}
	for (auto iter = pushed.begin(); iter != pushed.end(); iter++)
		m_Slots[*iter].FlushPush();
	Sort();
}

void DebugBatcher::PushBackAll(std::vector<std::shared_ptr<DebugPolygon>>&& polys)
{
	std::unordered_set<std::pair<GLenum, BatchModel>> pushed;
	DebugModel model;
	for (const auto& poly : polys)
	{
		model = poly->GetDebugModel();
		pushed.insert(model);
		const auto& multi_polygon = m_Slots.find(model);
		if (multi_polygon != m_Slots.end())
			multi_polygon->second.BufferPush(poly);
		else
		{
			DebugMultiPolygon multi(model);
			multi.BufferPush(poly);
			m_Slots.emplace(model, std::move(multi));
		}
	}
	for (auto iter = pushed.begin(); iter != pushed.end(); iter++)
		m_Slots[*iter].FlushPush();
	Sort();
}

bool DebugBatcher::Erase(const std::vector<std::shared_ptr<DebugPolygon>>::iterator& where)
{
	DebugModel model = (*where)->GetDebugModel();
	const auto& multi_polygon = m_Slots.find(model);
	if (multi_polygon != m_Slots.end())
	{
		multi_polygon->second.Erase(where);
		if (multi_polygon->second.draw_count == 0)
		{
			m_Slots.erase(multi_polygon);
			Sort();
		}
		return true;
	}
	return false;
}

void DebugBatcher::EraseAll(const std::unordered_map<DebugModel, std::unordered_set<std::shared_ptr<DebugPolygon>>>& polys)
{
	for (auto iter = polys.begin(); iter != polys.end(); iter++)
	{
		auto multi_polygon = m_Slots.find(iter->first);
		if (multi_polygon != m_Slots.end())
		{
			multi_polygon->second.EraseAll(iter->second);
			if (multi_polygon->second.draw_count == 0)
			{
				m_Slots.erase(multi_polygon);
			}
		}
	}
	Sort();
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

size_t DebugBatcher::Size() const
{
	size_t size = 0;
	for (const auto& slot : m_Slots)
		size += slot.second.m_Polygons.size();
	return size;
}

void DebugBatcher::Sort()
{
	m_OrderedTraversal.clear();
	for (auto& pair : m_Slots)
		m_OrderedTraversal.push_back(&pair.second);
	std::stable_sort(m_OrderedTraversal.begin(), m_OrderedTraversal.end(),
		[](const DebugMultiPolygon* const first, const DebugMultiPolygon* const second)
		{
			return first->GetZIndex() < second->GetZIndex();
		});
}
