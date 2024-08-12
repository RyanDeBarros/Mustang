#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "utils/Constants.h"
#include "../../Renderable.h"
#include "../../ActorRenderBase.h"
#include "../../transform/Transformable.h"
#include "../../transform/Modulatable.h"

typedef std::pair<GLenum, BatchModel> DebugModel;

template<>
struct std::hash<DebugModel>
{
	inline size_t operator()(const DebugModel& model) const
	{
		auto hash1 = hash<GLenum>{}(model.first);
		auto hash2 = hash<BatchModel>{}(model.second);
		return hash1 ^ (hash2 << 1);
	}
};

class DebugPolygon : public ActorRenderBase2D
{
protected:
	friend class DebugMultiPolygon;
	friend class DebugBatcher;

	friend class CanvasLayer;
	Renderable m_Renderable;
	std::vector<glm::vec2> m_Points;
	std::shared_ptr<class DebugModulatable> m_Color;
	GLenum m_IndexingMode;
	std::shared_ptr<class DebugTransformable2D> m_Transform;

	// 0b ... transform RS | transform P | point positions | color | indexing mode or points vector
	unsigned char m_Status = 0b11111;

	virtual void CheckStatus();

public:
	DebugPolygon(const std::vector<glm::vec2>& points, const Transform2D& transform = {}, const glm::vec4& color = Colors::WHITE, GLenum indexing_mode = GL_LINE_STRIP, ZIndex z = 0);
	DebugPolygon(const DebugPolygon&);
	DebugPolygon(DebugPolygon&&) noexcept;
	DebugPolygon& operator=(const DebugPolygon&);
	DebugPolygon& operator=(DebugPolygon&&) noexcept;

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	virtual bool DrawPrep();
	inline DebugModel GetDebugModel() const { return { m_IndexingMode, m_Renderable.model }; }

	inline std::vector<glm::vec2>& PointsRef() { m_Status |= 0b1; return m_Points; }
	inline void SetPointPosition(const size_t& i, const glm::vec2& point) { if (i >= 0 && i < m_Points.size()) { m_Points[i] = point; m_Status |= 0b100; } }

	inline GLenum GetIndexingMode() const { return m_IndexingMode; }
	inline void SetIndexingMode(GLenum indexing_mode) { m_Status |= 0b1; m_IndexingMode = indexing_mode; }

	inline void FlushTransform() { m_Status |= 0b11000; }
	inline void FlushPosition() { m_Status |= 0b1000; }
	inline void FlushRotation() { m_Status |= 0b10000; }
	inline void FlushScale() { m_Status |= 0b10000; }
	inline void FlushColor() { m_Status |= 0b10; }

	bool visible = true;

	std::shared_ptr<DebugTransformable2D> Transform() { return m_Transform; }
	std::weak_ptr<DebugTransformable2D> TransformWeak() { return m_Transform; }
	std::shared_ptr<DebugModulatable> Modulate() { return m_Color; }
	std::weak_ptr<DebugModulatable> ModulateWeak() { return m_Color; }
};

class DebugTransformable2D : public TransformableProxy2D
{
	friend class DebugPolygon;
	DebugPolygon* m_Poly = nullptr;

public:
	DebugTransformable2D(const Transform2D& transform = {}) : TransformableProxy2D(transform) {}

	inline virtual void OperateTransform(const std::function<void(Transform2D& transform)>& op) override { op(m_Transform); m_Poly->FlushTransform(); }
	inline virtual void OperatePosition(const std::function<void(glm::vec2& position)>& op) override { op(m_Transform.position); m_Poly->FlushPosition(); }
	inline virtual void OperateRotation(const std::function<void(glm::float32& rotation)>& op) override { op(m_Transform.rotation); m_Poly->FlushRotation(); }
	inline virtual void OperateScale(const std::function<void(glm::vec2& scale)>& op) override { op(m_Transform.scale); m_Poly->FlushScale(); }
};

class DebugModulatable : public ModulatableProxy
{
	friend class DebugPolygon;
	DebugPolygon* m_Poly = nullptr;

public:
	DebugModulatable(const glm::vec4& color = Colors::WHITE) : ModulatableProxy(color) {}

	inline virtual void OperateColor(const std::function<void(glm::vec4& color)>& op) override { op(m_Color); m_Poly->FlushColor(); }
};
