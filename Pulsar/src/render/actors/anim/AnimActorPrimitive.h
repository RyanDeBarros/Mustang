#pragma once

#include "../ActorPrimitive.h"
#include "FramesArray.h"

class AnimActorPrimitive2D
{
	ActorPrimitive2D* m_Primitive = nullptr;
	Array<FramesArray> m_Anims;
	float m_FrameLength = 0.05f;
	float m_TimeElapsed = 0.0f;
	float m_SpeedScale = 1.0f;
	unsigned short m_CurrentAnimIndex;

public:
	AnimActorPrimitive2D(ActorPrimitive2D* heap_primitive, Array<FramesArray>&& anims, float frame_length = 0.0f, bool play_in_reverse = false, float speed_scale = 1.0f);
	~AnimActorPrimitive2D();
	
	bool m_PlayInReverse;

	ActorPrimitive2D* Primitive() { return m_Primitive; }
	ActorPrimitive2D const* Primitive() const { return m_Primitive; }
	FramesArray* CurrentAnim() { return &m_Anims[m_CurrentAnimIndex]; }
	unsigned short CurrentAnimIndex() const { return m_CurrentAnimIndex; }
	void SetAnimIndex(unsigned short anim);
	float FrameLength() const { return m_FrameLength; }
	void SetFrameLength(float frame_length) { if (frame_length >= 0.0f) m_FrameLength = frame_length; }
	void SelectFrame(unsigned short frame_index);
	float SpeedScale() const { return m_SpeedScale; }
	void SetSpeedScale(float speed_scale) { if (speed_scale >= 0.0f) m_SpeedScale = speed_scale; }

	void OnUpdate();
	
private:
	void SyncTexture();
};
