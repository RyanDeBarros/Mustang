#version 440 core

layout(location=0) in vec2 i_TransformP;
layout(location=1) in vec4 i_TransformRS;
layout(location=2) in vec4 i_Color;
layout(location=3) in vec2 i_Position;
layout(location=4) in float i_PointSize;

uniform mat3 u_VP = mat3(vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0));

out vec4 t_Color;

void main() {
	t_Color = i_Color;
	
	// model matrix
	mat3 M = mat3(vec3(i_TransformRS[0], i_TransformRS[1], 0.0), vec3(i_TransformRS[2], i_TransformRS[3], 0.0), vec3(i_TransformP[0], i_TransformP[1], 1.0));
	gl_Position.xy = (u_VP * M * vec3(i_Position, 1.0)).xy;
	gl_PointSize = i_PointSize;
}
