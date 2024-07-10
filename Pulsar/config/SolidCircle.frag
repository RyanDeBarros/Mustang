#version 440 core

layout(location=0) out vec4 o_Color;

in float t_InnerRadius;
in vec4 t_InnerColor, t_OuterColor;

void main() {
	if (length(gl_PointCoord - vec2(0.5)) > 0.5) {
		discard;
	} else if (length(gl_PointCoord - vec2(0.5)) >= t_InnerRadius) {
		o_Color = t_OuterColor;
	} else {
		o_Color = t_InnerColor;
	}
}
