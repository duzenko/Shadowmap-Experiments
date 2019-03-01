R"(#version 130

uniform mat4 viewMatrix, projectionMatrix;

out vec4 clipSpace;

void main() {
	gl_FrontColor = gl_Color;
	gl_Position = clipSpace = projectionMatrix * viewMatrix * gl_Vertex;
}
)"