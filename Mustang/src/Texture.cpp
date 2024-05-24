#include "Texture.h"

#include <string>
#include <GL/glew.h>

#include "stb_image/stb_image.h"

#include "Logger.h"
#include "Utility.h"

Texture::Texture(const char* filepath, TextureSettings settings, GLint lod_level)
	: m_RID(0), m_Width(0), m_Height(0), m_BPP(0)
{
	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(filepath, &m_Width, &m_Height, &m_BPP, 4);

	if (!m_LocalBuffer)
	{
		Logger::LogWarning("Texture '" + std::string(filepath) + "' could not be loaded!\n" + stbi_failure_reason());
		return;
	}

	TRY(glGenTextures(1, &m_RID));

	Bind();
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)settings.min_filter));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)settings.mag_filter));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)settings.wrap_s));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)settings.wrap_t));
	TRY(glTexImage2D(GL_TEXTURE_2D, lod_level, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
	Unbind();
	stbi_image_free(m_LocalBuffer);
}

Texture::~Texture()
{
	TRY(glDeleteTextures(1, &m_RID));
}

void Texture::Bind(TextureSlot slot) const
{
	TRY(glActiveTexture(GL_TEXTURE0 + slot));
	TRY(glBindTexture(GL_TEXTURE_2D, m_RID));
	//TRY(glBindTextureUnit(slot, m_RID));
}

void Texture::Unbind(TextureSlot slot) const
{
	TRY(glActiveTexture(GL_TEXTURE0 + slot));
	TRY(glBindTexture(GL_TEXTURE_2D, 0));
	//TRY(glBindTextureUnit(slot, 0));
}
