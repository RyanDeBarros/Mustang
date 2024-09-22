#include "VendorInclude.h"

#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable : 4244)
#include <nanosvg/nanosvg.h>
#include <nanosvg/nanosvgrast.h>
#pragma warning(pop)
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <stb/stb_truetype.h>
