#include "AnimActorPrimitive.h"

#include "Pulsar.h"
#include "utils/CommonMath.h"

AnimActorPrimitive2D::AnimActorPrimitive2D(ActorPrimitive2D* heap_primitive, Array<FramesArray>&& anims, float frame_length, bool play_in_reverse, float speed_scale)
	: m_Primitive(heap_primitive), m_Anims(std::move(anims)), m_PlayInReverse(play_in_reverse)
{
	SetFrameLength(frame_length);
	SetSpeedScale(speed_scale);
	SyncTexture();
}

AnimActorPrimitive2D::~AnimActorPrimitive2D()
{
	if (m_Primitive)
		delete m_Primitive;
}

void AnimActorPrimitive2D::SetAnimIndex(unsigned short anim)
{
	if (anim < m_Anims.Size())
		m_CurrentAnimIndex = anim;
	SyncTexture();
}

void AnimActorPrimitive2D::SelectFrame(unsigned short frame_index)
{
	m_Anims[m_CurrentAnimIndex].Select(frame_index);
	SyncTexture();
}

void AnimActorPrimitive2D::OnUpdate()
{
	if (m_FrameLength > 0.0f)
	{
		m_TimeElapsed += m_SpeedScale * Pulsar::deltaDrawTime;
		if (m_TimeElapsed > m_FrameLength)
		{
			auto& currentAnim = m_Anims[m_CurrentAnimIndex];
			if (m_PlayInReverse)
				currentAnim.Select(unsigned_mod(currentAnim.CurrentIndex() - static_cast<short>(m_TimeElapsed / m_FrameLength), currentAnim.Size()));
			else
				currentAnim.Select(unsigned_mod(currentAnim.CurrentIndex() + static_cast<short>(m_TimeElapsed / m_FrameLength), currentAnim.Size()));
			SyncTexture();
			m_TimeElapsed = std::fmod(m_TimeElapsed, m_FrameLength);
		}
	}
}

void AnimActorPrimitive2D::SyncTexture()
{
	if (m_Primitive)
		m_Primitive->SetTextureHandle(m_Anims[m_CurrentAnimIndex].CurrentTexture());
}
