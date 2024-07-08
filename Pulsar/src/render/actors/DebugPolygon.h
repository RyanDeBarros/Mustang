#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "render/Renderable.h"
#include "render/ActorRenderBase.h"
#include "render/Transform.h"

class DebugPolygon : public ActorRenderBase2D, public Transformable2D
{
	ZIndex m_Z;
	friend class CanvasLayer;
	Renderable m_Renderable;
	std::vector<glm::vec2> m_Points;
	glm::vec4 m_Color;
	GLenum m_IndexingMode;
	float m_PointSize;

	// 0b ... point size | transform RS | transform P | point positions | color | indexing mode or points vector
	unsigned char m_Status = 0b111111;
	Transform2D m_Transform;

public:
	DebugPolygon(const std::vector<glm::vec2>& points, const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f }, const GLenum& indexing_mode = GL_LINE_STRIP, const float& point_size = 2.0f, const ZIndex& z = 0);

	void RequestDraw(class CanvasLayer* canvas_layer);
	ZIndex GetZIndex() const { return m_Z; }
	void SetZIndex(const ZIndex& z) { m_Z = z; }

	inline std::vector<glm::vec2>& PointsRef() { m_Status |= 0b1; return m_Points; }
	inline void SetPostion(const size_t& i, const glm::vec2& point) { if (i >= 0 && i < m_Points.size()) { m_Points[i] = point; m_Status |= 0b100; } }

	inline GLenum GetIndexingMode() const { return m_IndexingMode; }
	inline void SetIndexingMode(const GLenum& indexing_mode) { m_Status |= 0b1; m_IndexingMode = indexing_mode; }
	inline glm::vec4 GetColor() const { return m_Color; }
	inline void SetColor(const glm::vec4& color) { m_Status |= 0b10; m_Color = color; }
	inline float GetPointSize() const { return m_PointSize; }
	inline void SetPointSize(const float& point_size) { m_Status |= 0b100000; m_PointSize = point_size; }

	inline Transform2D GetTransform() const override { return m_Transform; }
	inline void SetTransform(const Transform2D& transform) override { m_Transform = transform; m_Status |= 0b11000; }
	inline glm::vec2 GetPosition() const override { return m_Transform.position; }
	inline void OperatePosition(const std::function<void(glm::vec2& position)>& op) override { op(m_Transform.position); m_Status |= 0b1000; }
	inline glm::float32 GetRotation() const override { return m_Transform.rotation; }
	inline void OperateRotation(const std::function<void(glm::float32& rotation)>& op) override { op(m_Transform.rotation); m_Status |= 0b10000; }
	inline glm::vec2 GetScale() const override { return m_Transform.scale; }
	inline void OperateScale(const std::function<void(glm::vec2& scale)>& op) override { op(m_Transform.scale); m_Status |= 0b10000; }

	bool visible = true;
};
