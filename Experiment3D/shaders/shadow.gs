R"(#version 150

layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

out vec4 clipSpace;

uniform int pageSize;

void main() {
	vec2 f1 = gl_in[0].gl_Position.xy / gl_in[0].gl_Position.w;
	vec2 f2 = gl_in[1].gl_Position.xy / gl_in[1].gl_Position.w;
	vec2 x;
	x[0] = min(f1.x, f2.x);
	x[1] = max(f1.x, f2.x);
	x = x * .5 + .5;
	x = fract(pageSize*x + .5);
	gl_Position = gl_in[0].gl_Position;
	clipSpace.xy = x;
	EmitVertex();
	gl_Position = gl_in[1].gl_Position;
	clipSpace.xy = x;
	EmitVertex();
	EndPrimitive();
})"