R"(#version 130

uniform mat4 viewMatrix, projectionMatrix;

void main() {
	gl_FrontColor = gl_Color;
	gl_Position = projectionMatrix * viewMatrix * gl_Vertex;
}
)"