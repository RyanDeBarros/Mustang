#pragma once

#include "VendorInclude.h"

enum class StandardCursor
{
	ARROW = GLFW_ARROW_CURSOR,
	IBEAM = GLFW_IBEAM_CURSOR,
	CROSSHAIR = GLFW_CROSSHAIR_CURSOR,
	HAND = GLFW_POINTING_HAND_CURSOR,
	RESIZE_EW = GLFW_RESIZE_EW_CURSOR,
	RESIZE_NS = GLFW_RESIZE_NS_CURSOR,
	RESIZE_NW_SE = GLFW_RESIZE_NWSE_CURSOR,
	RESIZE_NE_SW = GLFW_RESIZE_NESW_CURSOR,
	RESIZE_OMNI = GLFW_RESIZE_ALL_CURSOR,
	CANCEL = GLFW_NOT_ALLOWED_CURSOR
};

enum class MouseMode
{
	VISIBLE = GLFW_CURSOR_NORMAL,
	HIDDEN = GLFW_CURSOR_HIDDEN,
	VIRTUAL = GLFW_CURSOR_DISABLED,
	CAPTURED = GLFW_CURSOR_CAPTURED
};

class Cursor
{
	GLFWcursor* cursor = nullptr;

public:
	Cursor() = default;
	Cursor(StandardCursor standard_cursor);
	Cursor(unsigned char const* rgba_pixels, int width, int height, int xhot = 0, int yhot = 0);
	Cursor(const Cursor&) = delete;
	Cursor(Cursor&&) noexcept;
	Cursor& operator=(Cursor&&) noexcept;
	~Cursor();

	GLFWcursor* _GetInternal() { return cursor; }
	MouseMode mouseMode = MouseMode::VISIBLE;
	bool rawMouseMotion = false;
};
