#include "DebugMultiPolygon.h"

#include <algorithm>

#include "render/CanvasLayer.h"

DebugMultiPolygon::DebugMultiPolygon()
	: indexes_ptr(nullptr), index_counts_ptr(nullptr), draw_count(0), m_IndexMode(0), m_Model({}), ActorRenderBase2D(0)
{
}

DebugMultiPolygon::DebugMultiPolygon(const std::pair<GLenum, BatchModel>& pair, const ZIndex& z)
	: indexes_ptr(nullptr), index_counts_ptr(nullptr), draw_count(0), m_IndexMode(pair.first), m_Model(pair.second), ActorRenderBase2D(z)
{
}

DebugMultiPolygon::DebugMultiPolygon(const DebugMultiPolygon& other)
	: indexes_ptr(nullptr), index_counts_ptr(nullptr), draw_count(other.draw_count), m_IndexMode(other.m_IndexMode),
	m_Model(other.m_Model), m_Polygons(other.m_Polygons), ActorRenderBase2D(other)
{
	if (other.indexes_ptr)
	{
		indexes_ptr = new GLint[draw_count];
		memcpy_s(indexes_ptr, draw_count, other.indexes_ptr, draw_count);
	}
	if (other.index_counts_ptr)
	{
		index_counts_ptr = new GLsizei[draw_count];
		memcpy_s(index_counts_ptr, draw_count, other.index_counts_ptr, draw_count);
	}
}

DebugMultiPolygon::DebugMultiPolygon(DebugMultiPolygon&& other) noexcept
	: indexes_ptr(other.indexes_ptr), index_counts_ptr(other.index_counts_ptr), draw_count(other.draw_count), m_IndexMode(other.m_IndexMode),
	m_Model(other.m_Model), m_Polygons(std::move(other.m_Polygons)), ActorRenderBase2D(std::move(other))
{
	other.indexes_ptr = nullptr;
	other.index_counts_ptr = nullptr;
}

DebugMultiPolygon& DebugMultiPolygon::operator=(const DebugMultiPolygon& other)
{
	draw_count = other.draw_count;
	m_IndexMode = other.m_IndexMode;
	m_Model = other.m_Model;
	m_Polygons = other.m_Polygons;

	if (indexes_ptr)
		delete[] indexes_ptr;
	if (other.indexes_ptr)
	{
		indexes_ptr = new GLint[draw_count];
		memcpy_s(indexes_ptr, draw_count, other.indexes_ptr, draw_count);
	}
	else
		indexes_ptr = nullptr;

	if (index_counts_ptr)
		delete[] index_counts_ptr;
	if (other.index_counts_ptr)
	{
		index_counts_ptr = new GLsizei[draw_count];
		memcpy_s(index_counts_ptr, draw_count, other.index_counts_ptr, draw_count);
	}
	else
		index_counts_ptr = nullptr;
	return *this;
}

DebugMultiPolygon& DebugMultiPolygon::operator=(DebugMultiPolygon&& other) noexcept
{
	draw_count = other.draw_count;
	m_IndexMode = other.m_IndexMode;
	m_Model = other.m_Model;
	m_Polygons = std::move(other.m_Polygons);

	if (indexes_ptr)
		delete[] indexes_ptr;
	indexes_ptr = other.indexes_ptr;
	other.indexes_ptr = nullptr;

	if (index_counts_ptr)
		delete[] index_counts_ptr;
	index_counts_ptr = other.index_counts_ptr;
	other.index_counts_ptr = nullptr;

	return *this;
}

DebugMultiPolygon::~DebugMultiPolygon()
{
	if (indexes_ptr)
		delete[] indexes_ptr;
	if (index_counts_ptr)
		delete[] index_counts_ptr;
}

void DebugMultiPolygon::RequestDraw(CanvasLayer* canvas_layer)
{
	canvas_layer->DrawMultiArray(*this);
}

void DebugMultiPolygon::Sort()
{
	std::sort(m_Polygons.begin(), m_Polygons.end(),
		[](const std::shared_ptr<DebugPolygon>& first, const std::shared_ptr<DebugPolygon>& second)
		{
			return first->GetZIndex() < second->GetZIndex();
		});
}

void DebugMultiPolygon::ChangeZIndex(const iterator& where, ZIndex z)
{
	(*where)->SetZIndex(z);
	Sort();
	UpdatePtrs();
}

void DebugMultiPolygon::ChangeZIndex(size_t i, ZIndex z)
{
	m_Polygons[i]->SetZIndex(z);
	Sort();
	UpdatePtrs();
}

void DebugMultiPolygon::PushBack(const std::shared_ptr<DebugPolygon>& poly)
{
	m_Polygons.push_back(poly);
	Sort();
	UpdatePtrs();
}

void DebugMultiPolygon::PushBack(std::shared_ptr<DebugPolygon>&& poly)
{
	m_Polygons.push_back(std::move(poly));
	Sort();
	UpdatePtrs();
}

void DebugMultiPolygon::PushBackAll(const std::vector<std::shared_ptr<DebugPolygon>>& polys)
{
	for (auto& poly : polys)
		m_Polygons.push_back(poly);
	Sort();
	UpdatePtrs();
}

// TODO might be an issue if the shared_ptr elements in vectors continued to be used after PushBackAll, even though vector isn't.
void DebugMultiPolygon::PushBackAll(std::vector<std::shared_ptr<DebugPolygon>>&& polys)
{
	for (auto&& poly : polys)
		m_Polygons.push_back(std::move(poly));
	Sort();
	UpdatePtrs();
}

void DebugMultiPolygon::BufferPush(const std::shared_ptr<DebugPolygon>& poly)
{
	m_Polygons.push_back(poly);
}

void DebugMultiPolygon::BufferPush(std::shared_ptr<DebugPolygon>&& poly)
{
	m_Polygons.push_back(std::move(poly));
}

void DebugMultiPolygon::FlushPush()
{
	Sort();
	UpdatePtrs();
}

void DebugMultiPolygon::Erase(const iterator& where)
{
	m_Polygons.erase(where);
	UpdatePtrs();
}

void DebugMultiPolygon::Erase(size_t i)
{
	m_Polygons.erase(m_Polygons.begin() + i);
	UpdatePtrs();
}

void DebugMultiPolygon::EraseAll(const std::vector<size_t>& is)
{
	for (const auto& i : is)
		m_Polygons.erase(m_Polygons.begin() + i);
	UpdatePtrs();
}

void DebugMultiPolygon::EraseAll(const std::unordered_set<std::shared_ptr<DebugPolygon>>& polys)
{
	std::erase_if(m_Polygons, [&](const std::shared_ptr<DebugPolygon>& poly) { return polys.find(poly) != polys.end(); });
	UpdatePtrs();
}

DebugMultiPolygon::iterator DebugMultiPolygon::Find(const std::shared_ptr<DebugPolygon>& poly)
{
	return std::find(m_Polygons.begin(), m_Polygons.end(), poly);
}

DebugMultiPolygon::const_iterator DebugMultiPolygon::PolyBegin()
{
	return m_Polygons.begin();
}

void DebugMultiPolygon::UpdatePtrs() const
{
	if (indexes_ptr)
		delete[] indexes_ptr;
	if (index_counts_ptr)
		delete[] index_counts_ptr;
	draw_count = static_cast<GLsizei>(m_Polygons.size());
	indexes_ptr = new GLint[draw_count];
	index_counts_ptr = new GLsizei[draw_count];
	for (size_t i = 0; i < draw_count; i++)
	{
		index_counts_ptr[i] = static_cast<GLsizei>(m_Polygons[i]->m_Points.size());
		if (i == 0)
			indexes_ptr[i] = 0;
		else
			indexes_ptr[i] = static_cast<GLint>(indexes_ptr[i - 1] + index_counts_ptr[i - 1]);
	}
}
