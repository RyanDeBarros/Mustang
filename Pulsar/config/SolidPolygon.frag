#version 440 core

layout(location=0) out vec4 o_Color;

in vec4 t_Color;

void main() {
	o_Color = t_Color;
}
