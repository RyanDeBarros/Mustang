#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "utils/Constants.h"
#include "../../Renderable.h"
#include "../../ActorRenderBase.h"
#include "../../transform/Transform.h"
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

	Transformer2D m_Transformer;

	// 0b ... transform RS | transform P | point positions | color | indexing mode or points vector
	unsigned char m_Status = 0b11111;

	virtual void CheckStatus();

public:
	DebugPolygon(const std::vector<glm::vec2>& points, const Transform2D& transform = {}, const glm::vec4& color = Colors::WHITE, GLenum indexing_mode = GL_LINE_STRIP, ZIndex z = 0);
	DebugPolygon(const DebugPolygon&);
	DebugPolygon(DebugPolygon&&) noexcept;
	DebugPolygon& operator=(const DebugPolygon&);
	DebugPolygon& operator=(DebugPolygon&&) noexcept;
	~DebugPolygon();

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	virtual bool DrawPrep();
	inline DebugModel GetDebugModel() const { return { m_IndexingMode, m_Renderable.model }; }

	inline std::vector<glm::vec2>& PointsRef() { m_Status |= 0b1; return m_Points; }
	inline void SetPointPosition(const size_t& i, const glm::vec2& point) { if (i >= 0 && i < m_Points.size()) { m_Points[i] = point; m_Status |= 0b100; } }

	inline GLenum GetIndexingMode() const { return m_IndexingMode; }
	inline void SetIndexingMode(GLenum indexing_mode) { m_Status |= 0b1; m_IndexingMode = indexing_mode; }

	inline void FlagTransform() { m_Status |= 0b11000; }
	inline void FlagTransformP() { m_Status |= 0b1000; }
	inline void FlagTransformRS() { m_Status |= 0b10000; }

	inline void FlushColor() { m_Status |= 0b10; }

	bool visible = true;

	Transformer2D* Transformer() { return &m_Transformer; }
	std::shared_ptr<DebugModulatable> Modulate() { return m_Color; }
	std::weak_ptr<DebugModulatable> ModulateWeak() { return m_Color; }
};

struct DebugPolygon_TN : public TransformNotification
{
	DebugPolygon* poly = nullptr;

	DebugPolygon_TN(DebugPolygon* poly) : poly(poly) {}

	void Notify() override { if (poly) poly->FlagTransform(); }
	void NotifyP() override { if (poly) poly->FlagTransformP(); }
	void NotifyRS() override { if (poly) poly->FlagTransformRS(); }
};

class DebugModulatable : public ModulatableProxy
{
	friend class DebugPolygon;
	DebugPolygon* m_Poly = nullptr;

public:
	DebugModulatable(const glm::vec4& color = Colors::WHITE) : ModulatableProxy(color) {}

	inline virtual void OperateColor(const std::function<void(glm::vec4& color)>& op) override { op(m_Color); m_Poly->FlushColor(); }
};
