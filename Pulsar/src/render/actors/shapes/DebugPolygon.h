#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../../Renderable.h"
#include "../../ActorRenderBase.h"
#include "../../transform/Transform.h"
#include "../../transform/Modulate.h"

typedef std::pair<GLenum, BatchModel> DebugModel;

template<>
struct std::hash<DebugModel>
{
	size_t operator()(const DebugModel& model) const
	{
		auto hash1 = hash<GLenum>{}(model.first);
		auto hash2 = hash<BatchModel>{}(model.second);
		return hash1 ^ (hash2 << 1);
	}
};

struct DP_Notification;

class DebugPolygon : public FickleActor2D
{
protected:
	friend class DebugMultiPolygon;
	friend class DebugBatcher;

	friend class CanvasLayer;
	Renderable m_Renderable;
	std::vector<glm::vec2> m_Points;

	DP_Notification* m_Notification;

	GLenum m_IndexingMode;

	// 0b ... transform RS | transform P | point positions | color | indexing mode or points vector
	unsigned char m_Status = 0b11111;

	virtual void CheckStatus();

public:
	DebugPolygon(const std::vector<glm::vec2>& points, GLenum indexing_mode = GL_LINE_STRIP, ZIndex z = 0, FickleType fickle_type = FickleType::Protean);
	DebugPolygon(const DebugPolygon&);
	DebugPolygon(DebugPolygon&&) noexcept;
	DebugPolygon& operator=(const DebugPolygon&);
	DebugPolygon& operator=(DebugPolygon&&) noexcept;
	~DebugPolygon();

	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;

	virtual bool DrawPrep();
	DebugModel GetDebugModel() const { return { m_IndexingMode, m_Renderable.model }; }

	std::vector<glm::vec2>& PointsRef() { m_Status |= 0b1; return m_Points; }
	void SetPointPosition(const size_t& i, const glm::vec2& point) { if (i >= 0 && i < m_Points.size()) { m_Points[i] = point; m_Status |= 0b100; } }

	GLenum GetIndexingMode() const { return m_IndexingMode; }
	void SetIndexingMode(GLenum indexing_mode) { m_Status |= 0b1; m_IndexingMode = indexing_mode; }

	void FlagProteate() { m_Status |= 0b11010; }
	void FlagTransform() { m_Status |= 0b11000; }
	void FlagTransformP() { m_Status |= 0b1000; }
	void FlagTransformRS() { m_Status |= 0b10000; }
	void FlagModulate() { m_Status |= 0b10; }

	bool visible = true;

private:
	void BindBufferFuncs();
	void (DebugPolygon::*f_BufferPackedP)(Stride) = nullptr;
	void (DebugPolygon::*f_BufferPackedRS)(Stride) = nullptr;
	void (DebugPolygon::*f_BufferPackedM)(Stride) = nullptr;
	void buffer_packed_p(Stride);
	void buffer_packed_p_default(Stride);
	void buffer_packed_rs(Stride);
	void buffer_packed_rs_default(Stride);
	void buffer_packed_m(Stride);
	void buffer_packed_m_default(Stride);
	void buffer_packed_empty(Stride) {}
};

struct DP_Notification : public FickleNotification
{
	DebugPolygon* poly = nullptr;

	DP_Notification(DebugPolygon* poly) : poly(poly) {}

	void Notify(FickleSyncCode code) override
	{
		switch (code)
		{
		case FickleSyncCode::SyncAll:
			if (poly) poly->FlagProteate();
			break;
		case FickleSyncCode::SyncT:
			if (poly) poly->FlagTransform();
			break;
		case FickleSyncCode::SyncP:
			if (poly) poly->FlagTransformP();
			break;
		case FickleSyncCode::SyncRS:
			if (poly) poly->FlagTransformRS();
			break;
		case FickleSyncCode::SyncM:
			if (poly) poly->FlagModulate();
			break;
		}
	}

};
