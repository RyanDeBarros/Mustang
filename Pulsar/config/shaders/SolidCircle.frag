#version 440 core

layout(location=0) out vec4 o_Color;

in vec4 t_Color;
in vec2 t_LBPos;

void main() {
	if (length(t_LBPos) > 0.5) discard;
	o_Color = t_Color;
}
