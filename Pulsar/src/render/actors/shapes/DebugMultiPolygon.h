#pragma once

#include <vector>
#include <memory>

#include "DebugPolygon.h"
#include "render/Renderable.h"

// TODO Here, std::shared_ptr is used. It should be used more throughout project.

class DebugMultiPolygon
{
	friend class CanvasLayer;
	friend class DebugBatcher;

	std::vector<std::shared_ptr<DebugPolygon>> m_Polygons;
	BatchModel m_Model;
	GLenum m_IndexMode;

	mutable GLint* indexes_ptr;
	mutable GLsizei* index_counts_ptr;
	mutable GLsizei draw_count;

	void UpdatePtrs() const;
	void Sort();

public:
	DebugMultiPolygon();
	DebugMultiPolygon(const std::pair<GLenum, BatchModel>& pair);
	DebugMultiPolygon(const DebugMultiPolygon&) = delete;
	DebugMultiPolygon(DebugMultiPolygon&&) noexcept;
	~DebugMultiPolygon();

	void ChangeZIndex(const std::vector<std::shared_ptr<DebugPolygon>>::iterator& where, const ZIndex& z);
	void ChangeZIndex(const size_t& i, const ZIndex& z);
	void PushBack(const std::shared_ptr<DebugPolygon>& poly);
	void PushBackAll(const std::vector<std::shared_ptr<DebugPolygon>>& polys);
	void BufferPush(const std::shared_ptr<DebugPolygon>& poly);
	void FlushPush();
	void Erase(const std::vector<std::shared_ptr<DebugPolygon>>::iterator& where);
	void Erase(const size_t& i);
	void EraseAll(const std::vector<size_t>& is);
	std::vector<std::shared_ptr<DebugPolygon>>::iterator Find(const std::shared_ptr<DebugPolygon>& poly);
	std::vector<std::shared_ptr<DebugPolygon>>::const_iterator PolyBegin();
};
