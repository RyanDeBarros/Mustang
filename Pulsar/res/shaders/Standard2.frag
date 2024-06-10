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

	if (o_Color.w < 0.1) {
		o_Color.rgb = vec3(1.0, 0.0, 0.0);
	} else if (o_Color.w < 0.5) {
		o_Color.rgb = vec3(0.0, 1.0, 0.0);
	} else {
		o_Color.rgb = vec3(0.0, 0.0, 1.0);
	}
}
