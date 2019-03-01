R"(#version 150

layout (lines) in;
layout (line_strip, max_vertices = 2) out;

out vec4 clipSpace;

void main() {
	gl_Position = clipSpace = gl_PositionIn[0];
	EmitVertex();
	gl_Position = clipSpace = gl_PositionIn[1];
	EmitVertex();
	EndPrimitive();
})"