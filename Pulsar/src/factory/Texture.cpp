#include "Texture.h"

#include <string>
#include <GL/glew.h>

#include <stb_image/stb_image.h>

#include "Logger.h"
#include "Macros.h"

Texture::Texture(const char* filepath, TextureSettings settings)
	: m_RID(0), m_Width(0), m_Height(0), m_BPP(0)
{
	stbi_set_flip_vertically_on_load(1);
	unsigned char* image_buffer = stbi_load(filepath, &m_Width, &m_Height, &m_BPP, STBI_rgb_alpha);

	if (!image_buffer)
	{
		Logger::LogWarning("Texture '" + std::string(filepath) + "' could not be loaded!\n" + stbi_failure_reason());
		return;
	}

	TRY(glGenTextures(1, &m_RID));
	
	// TODO remove these lines
	TRY(glEnable(GL_BLEND));
	TRY(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	TRY(glDepthMask(GL_FALSE));

	TRY(glBindTexture(GL_TEXTURE_2D, m_RID));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)settings.min_filter));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)settings.mag_filter));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)settings.wrap_s));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)settings.wrap_t));
	TRY(glTexImage2D(GL_TEXTURE_2D, (GLint)settings.lod_level, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_buffer));
	TRY(glBindTexture(GL_TEXTURE_2D, 0));
	stbi_image_free(image_buffer);
}

Texture::~Texture()
{
	TRY(glDeleteTextures(1, &m_RID));
}

void Texture::Bind(TextureSlot slot) const
{
	TRY(glActiveTexture(GL_TEXTURE0 + slot));
	TRY(glBindTexture(GL_TEXTURE_2D, m_RID));
}

void Texture::Unbind(TextureSlot slot) const
{
	TRY(glActiveTexture(GL_TEXTURE0 + slot));
	TRY(glBindTexture(GL_TEXTURE_2D, 0));
}
