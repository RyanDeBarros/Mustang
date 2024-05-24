#version 440 core

layout(location=0) in vec2 i_Position;
layout(location=1) in vec2 i_TexCoord;
layout(location=2) in float i_TexSlot;
layout(location=3) in vec4 i_Color;

out vec4 t_Color;
out float t_TexSlot;
out vec2 t_TexCoord;

void main() {
	gl_Position = vec4(i_Position, 0.0, 1.0);
	t_Color = i_Color;
	t_TexCoord = i_TexCoord;
	t_TexSlot = i_TexSlot;
}
