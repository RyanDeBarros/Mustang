#pragma once

#include "../ActorPrimitive.h"
#include "FramesArray.h"

class AnimActorPrimitive2D
{
	ActorPrimitive2D* m_Primitive = nullptr;
	Array<FramesArray> m_Anims;
	size_t m_CurrentAnim;
	float m_FrameLength;

public:
	AnimActorPrimitive2D(ActorPrimitive2D* heap_primitive, Array<FramesArray>&& anims);
	~AnimActorPrimitive2D();

	inline ActorPrimitive2D* Primitive() { return m_Primitive; }
	inline ActorPrimitive2D const* Primitive() const { return m_Primitive; }
	inline FramesArray* CurrentAnim() { return &m_Anims[m_CurrentAnim]; }
	inline size_t CurrentAnimIndex() const { return m_CurrentAnim; }
	void SetAnimIndex(size_t anim);
	inline float FrameLength() const { return m_FrameLength; }
	void SetFrameLength(float frame_length);
	void SelectFrame(unsigned short frame_index);

	void OnUpdate();
	
private:
	void SyncTexture();
};
