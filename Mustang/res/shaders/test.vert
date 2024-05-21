#version 440 core

layout(location=0) in vec2 i_Position;
layout(location=1) in vec4 i_Color;

out vec4 t_Color;

void main() {
	gl_Position = vec4(i_Position, 0.0, 1.0);
	t_Color = i_Color;
}
