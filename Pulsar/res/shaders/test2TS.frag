#version 440 core

layout(location=0) out vec4 o_Color;

in vec4 t_Color;
in float t_TexSlot;
in vec2 t_TexCoord;

layout(binding=0) uniform sampler2D TEXTURE_SLOTS[2];

void main() {
	if (t_TexSlot < 0) {
		o_Color = t_Color;
	} else {
		o_Color = texture(TEXTURE_SLOTS[int(t_TexSlot)], t_TexCoord);
	}
}
