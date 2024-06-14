#version 440 core

layout(location=0) out vec4 o_Color;

in vec4 t_Color;
in float t_TexSlot;
in vec2 t_TexCoord;

uniform int u_inttest;
uniform vec3 u_float3test;

layout(binding=0) uniform sampler2D TEXTURE_SLOTS[32];

void main() {
	if (t_TexSlot < 0) {
		o_Color = t_Color;
	} else {
		o_Color = t_Color * texture(TEXTURE_SLOTS[int(t_TexSlot)], t_TexCoord);
		if (o_Color[3] == 1.0)
		{
			o_Color = vec4(u_float3test[0], u_float3test[1], u_float3test[2], 1.0);
		}
	}
}
