#pragma once

#include <vector>
#include <memory>
#include <unordered_set>

#include "DebugPolygon.h"
#include "render/Renderable.h"

class DebugMultiPolygon : public ActorRenderBase2D
{
	friend class CanvasLayer;
	friend class DebugBatcher;

	// TODO use regular pointer?
	std::vector<std::shared_ptr<DebugPolygon>> m_Polygons;
	BatchModel m_Model;
	GLenum m_IndexMode;

	mutable GLint* indexes_ptr;
	mutable GLsizei* index_counts_ptr;

	void UpdatePtrs() const;
	void Sort();

public:
	typedef std::vector<std::shared_ptr<DebugPolygon>>::iterator iterator;
	typedef std::vector<std::shared_ptr<DebugPolygon>>::const_iterator const_iterator;

	DebugMultiPolygon();
	DebugMultiPolygon(const std::pair<GLenum, BatchModel>& pair, const ZIndex& z = 0);
	DebugMultiPolygon(const DebugMultiPolygon&);
	DebugMultiPolygon(DebugMultiPolygon&&) noexcept;
	DebugMultiPolygon& operator=(const DebugMultiPolygon&);
	DebugMultiPolygon& operator=(DebugMultiPolygon&&) noexcept;
	~DebugMultiPolygon();

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	GLsizei DrawCount() const { return static_cast<GLsizei>(m_Polygons.size()); }
	void ChangeZIndex(const iterator& where, ZIndex z);
	void ChangeZIndex(size_t i, ZIndex z);
	void PushBack(const std::shared_ptr<DebugPolygon>& poly);
	void PushBack(std::shared_ptr<DebugPolygon>&& poly);
	void PushBackAll(const std::vector<std::shared_ptr<DebugPolygon>>& polys);
	void PushBackAll(std::vector<std::shared_ptr<DebugPolygon>>&& polys);
	void BufferPush(const std::shared_ptr<DebugPolygon>& poly);
	void BufferPush(std::shared_ptr<DebugPolygon>&& poly);
	void FlushPush();
	void Erase(const iterator& where);
	void Erase(size_t i);
	void EraseAll(const std::vector<size_t>& is);
	void EraseAll(const std::unordered_set<std::shared_ptr<DebugPolygon>>& polys);
	iterator Find(const std::shared_ptr<DebugPolygon>& poly);
	const_iterator PolyBegin();
};
