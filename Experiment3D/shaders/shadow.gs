R"(#version 150

layout (lines) in;
layout (line_strip, max_vertices = 2) out;

out vec4 clipSpace;

uniform int pageSize;

void main() {
	vec2 f1 = gl_PositionIn[0].xy / gl_PositionIn[0].w;
	vec2 f2 = gl_PositionIn[1].xy / gl_PositionIn[1].w;
	vec2 x;
	x[0] = min(f1.x, f2.x);
	x[1] = max(f1.x, f2.x);
	x = x * .5 + .5;
	x = fract(pageSize*x + .5);
	gl_Position = gl_PositionIn[0];
	clipSpace.xy = x;
	EmitVertex();
	gl_Position = gl_PositionIn[1];
	clipSpace.xy = x;
	EmitVertex();
	EndPrimitive();
})"