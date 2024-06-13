#include "Texture.h"

#include <string>
#include <GL/glew.h>

#include <stb_image/stb_image.h>

#include "Logger.h"
#include "Macros.h"

Texture::Texture(const char* filepath, TextureSettings settings)
	: m_RID(0), m_Width(0), m_Height(0), m_Filepath(filepath), m_Settings(settings)
{
	stbi_set_flip_vertically_on_load(1);
	int bpp;
	unsigned char* image_buffer = stbi_load(filepath, &m_Width, &m_Height, &bpp, STBI_rgb_alpha);

	if (!image_buffer)
	{
		Logger::LogWarning("Texture '" + m_Filepath + "' could not be loaded!\n" + stbi_failure_reason());
		return;
	}

	TRY(glGenTextures(1, &m_RID));
	TRY(glBindTexture(GL_TEXTURE_2D, m_RID));

	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)settings.min_filter));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)settings.mag_filter));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)settings.wrap_s));
	TRY(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)settings.wrap_t));
	TRY(glTexImage2D(GL_TEXTURE_2D, (GLint)settings.lod_level, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_buffer));

	TRY(glBindTexture(GL_TEXTURE_2D, 0));
	stbi_image_free(image_buffer);
}

Texture::Texture(Texture&& texture) noexcept
	: m_RID(texture.m_RID), m_Width(texture.m_Width), m_Height(texture.m_Height), m_Filepath(texture.m_Filepath), m_Settings(texture.m_Settings)
{
	texture.m_RID = 0;
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
