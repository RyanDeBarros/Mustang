#version 440 core

in vec4 t_Color;

layout(location=0) out vec4 o_Color;

void main() {
	o_Color = t_Color;
}
