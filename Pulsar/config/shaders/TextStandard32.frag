#version 440 core

layout(location=0) out vec4 o_Color;

in vec4 t_Color;
in float t_TexSlot;
in vec2 t_TexCoord;

layout(binding=0) uniform sampler2D TEXTURE_SLOTS[32];

void main() {
	if (t_TexSlot < 0) {
		o_Color = t_Color;
	} else {
		float grayscale = texture(TEXTURE_SLOTS[int(t_TexSlot)], t_TexCoord).r;
		o_Color = t_Color * vec4(1.0, 1.0, 1.0, grayscale);
		// TODO in bool (or make tex slot an int, with a binary combo). that determines whether to use line 16/18/19, or something else.
		//o_Color = t_Color * grayscale;
		//o_Color = t_Color * vec4(1.0 - grayscale, 1.0 - grayscale, 1.0 - grayscale, grayscale);
	}
}
