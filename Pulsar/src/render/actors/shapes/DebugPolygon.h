#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "render/Renderable.h"
#include "render/ActorRenderBase.h"
#include "render/Transform.h"

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

class DebugPolygon : public ActorRenderBase2D, public Transformable2D
{
protected:
	friend class DebugMultiPolygon;
	friend class DebugBatcher;

	friend class CanvasLayer;
	Renderable m_Renderable;
	std::vector<glm::vec2> m_Points;
	glm::vec4 m_Color;
	GLenum m_IndexingMode;

	// 0b ... transform RS | transform P | point positions | color | indexing mode or points vector
	unsigned char m_Status = 0b11111;

	virtual void CheckStatus();

public:
	DebugPolygon(const std::vector<glm::vec2>& points, const Transform2D& transform = {}, const glm::vec4 & color = { 1.0f, 1.0f, 1.0f, 1.0f }, const GLenum & indexing_mode = GL_LINE_STRIP, const ZIndex & z = 0);
	DebugPolygon(const DebugPolygon&);
	DebugPolygon(DebugPolygon&&) noexcept;

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	virtual bool DrawPrep();
	inline DebugModel GetDebugModel() const { return { m_IndexingMode, m_Renderable.model }; }

	inline std::vector<glm::vec2>& PointsRef() { m_Status |= 0b1; return m_Points; }
	inline void SetPointPosition(const size_t& i, const glm::vec2& point) { if (i >= 0 && i < m_Points.size()) { m_Points[i] = point; m_Status |= 0b100; } }

	inline GLenum GetIndexingMode() const { return m_IndexingMode; }
	inline void SetIndexingMode(const GLenum& indexing_mode) { m_Status |= 0b1; m_IndexingMode = indexing_mode; }
	inline glm::vec4 GetColor() const { return m_Color; }
	inline void SetColor(const glm::vec4& color) { m_Status |= 0b10; m_Color = color; }

	inline virtual void OperateTransform(const std::function<void(Transform2D& transform)>& op) override { op(*m_Transform); m_Status |= 0b11000; }
	inline virtual void OperatePosition(const std::function<void(glm::vec2& position)>& op) override { op(m_Transform->position); m_Status |= 0b1000; }
	inline virtual void OperateRotation(const std::function<void(glm::float32& rotation)>& op) override { op(m_Transform->rotation); m_Status |= 0b10000; }
	inline virtual void OperateScale(const std::function<void(glm::vec2& scale)>& op) override { op(m_Transform->scale); m_Status |= 0b10000; }

	bool visible = true;

private:
	template<std::unsigned_integral ParticleCount>
	friend class ParticleWave;
	inline std::shared_ptr<Transform2D> TransformRef() { return m_Transform; }
};
