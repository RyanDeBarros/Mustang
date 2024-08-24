#include "AnimActorPrimitive.h"

#include "Logger.inl"

AnimActorPrimitive2D::AnimActorPrimitive2D(ActorPrimitive2D* heap_primitive, Array<FramesArray>&& anims)
	: m_Primitive(heap_primitive), m_Anims(std::move(anims))
{
	SyncTexture();
}

AnimActorPrimitive2D::~AnimActorPrimitive2D()
{
	if (m_Primitive)
		delete m_Primitive;
}

void AnimActorPrimitive2D::SetAnimIndex(size_t anim)
{
	if (anim < m_Anims.Size())
		m_CurrentAnim = anim;
	SyncTexture();
}

void AnimActorPrimitive2D::SetFrameLength(float frame_length)
{
	m_FrameLength = frame_length >= 0.0f ? frame_length : -frame_length;
	if (m_FrameLength == 0.0f)
	{
		// TODO set func pointer for update?
	}
}

void AnimActorPrimitive2D::SelectFrame(unsigned short frame_index)
{
	m_Anims[m_CurrentAnim].Select(frame_index);
	SyncTexture();
}

void AnimActorPrimitive2D::OnUpdate()
{
	// TODO
}

void AnimActorPrimitive2D::SyncTexture()
{
	if (m_Primitive)
		m_Primitive->SetTextureHandle(m_Anims[m_CurrentAnim].CurrentTexture());
}
