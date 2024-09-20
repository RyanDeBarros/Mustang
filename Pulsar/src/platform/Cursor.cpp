#include "Cursor.h"

#include "Pulsar.h"

Cursor::Cursor(StandardCursor standard_cursor)
{
	cursor = glfwCreateStandardCursor(static_cast<int>(standard_cursor));
}

Cursor::Cursor(unsigned char const* rgba_pixels, int width, int height, int xhot, int yhot)
{
	GLFWimage image{ width, height, const_cast<unsigned char*>(rgba_pixels) };
	cursor = glfwCreateCursor(&image, xhot, yhot);
}

Cursor::Cursor(Cursor&& other) noexcept
	: cursor(other.cursor)
{
	other.cursor = nullptr;
}

Cursor& Cursor::operator=(Cursor&& other) noexcept
{
	if (this == &other)
		return *this;
	if (cursor && Pulsar::GLFWInitialized())
		glfwDestroyCursor(cursor);
	cursor = other.cursor;
	other.cursor = nullptr;
	return *this;
}

Cursor::~Cursor()
{
	if (cursor && Pulsar::GLFWInitialized())
		glfwDestroyCursor(cursor);
}
