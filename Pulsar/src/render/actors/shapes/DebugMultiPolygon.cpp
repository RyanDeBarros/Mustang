#include "DebugMultiPolygon.h"

#include <algorithm>

DebugMultiPolygon::DebugMultiPolygon()
	: indexes_ptr(nullptr), index_counts_ptr(nullptr), draw_count(0), m_IndexMode(0), m_Model({})
{
}

DebugMultiPolygon::DebugMultiPolygon(const std::pair<GLenum, BatchModel>& pair)
	: indexes_ptr(nullptr), index_counts_ptr(nullptr), draw_count(0), m_IndexMode(pair.first), m_Model(pair.second)
{
}

DebugMultiPolygon::DebugMultiPolygon(DebugMultiPolygon&& other) noexcept
	: indexes_ptr(other.indexes_ptr), index_counts_ptr(other.index_counts_ptr), draw_count(other.draw_count), m_IndexMode(other.m_IndexMode), m_Model(other.m_Model), m_Polygons(other.m_Polygons)
{
	other.indexes_ptr = nullptr;
	other.index_counts_ptr = nullptr;
}

DebugMultiPolygon::~DebugMultiPolygon()
{
	if (indexes_ptr)
		delete[] indexes_ptr;
	if (index_counts_ptr)
		delete[] index_counts_ptr;
}

void DebugMultiPolygon::Sort()
{
	std::sort(m_Polygons.begin(), m_Polygons.end(),
		[](const std::shared_ptr<DebugPolygon>& first, const std::shared_ptr<DebugPolygon>& second)
		{
			return first->GetZIndex() < second->GetZIndex();
		});
}

void DebugMultiPolygon::ChangeZIndex(const std::vector<std::shared_ptr<DebugPolygon>>::iterator& where, const ZIndex& z)
{
	(*where)->SetZIndex(z);
	Sort();
	UpdatePtrs();
}

void DebugMultiPolygon::ChangeZIndex(const size_t& i, const ZIndex& z)
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

void DebugMultiPolygon::PushBackAll(const std::vector<std::shared_ptr<DebugPolygon>>& polys)
{
	for (auto& poly : polys)
		m_Polygons.push_back(poly);
	Sort();
	UpdatePtrs();
}

void DebugMultiPolygon::BufferPush(const std::shared_ptr<DebugPolygon>& poly)
{
	m_Polygons.push_back(poly);
}

void DebugMultiPolygon::FlushPush()
{
	Sort();
	UpdatePtrs();
}

void DebugMultiPolygon::Erase(const std::vector<std::shared_ptr<DebugPolygon>>::iterator& where)
{
	m_Polygons.erase(where);
	UpdatePtrs();
}

void DebugMultiPolygon::Erase(const size_t& i)
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

std::vector<std::shared_ptr<DebugPolygon>>::iterator DebugMultiPolygon::Find(const std::shared_ptr<DebugPolygon>& poly)
{
	return std::find(m_Polygons.begin(), m_Polygons.end(), poly);
}

std::vector<std::shared_ptr<DebugPolygon>>::const_iterator DebugMultiPolygon::PolyBegin()
{
	return m_Polygons.begin();
}

void DebugMultiPolygon::UpdatePtrs() const
{
	if (indexes_ptr)
		delete[] indexes_ptr;
	if (index_counts_ptr)
		delete[] index_counts_ptr;
	draw_count = m_Polygons.size();
	indexes_ptr = new GLint[draw_count];
	index_counts_ptr = new GLsizei[draw_count];
	auto iter = m_Polygons.begin();
	for (size_t i = 0; i < draw_count; i++)
	{
		index_counts_ptr[i] = static_cast<GLsizei>((*iter)->m_Points.size());
		if (i == 0)
			indexes_ptr[i] = 0;
		else
			indexes_ptr[i] = static_cast<GLint>(indexes_ptr[i - 1] + index_counts_ptr[i - 1]);
		iter++;
	}
}
