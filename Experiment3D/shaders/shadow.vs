R"(#version 130

uniform mat4 modelMatrix, viewMatrix, projectionMatrix;

void main() {
	gl_FrontColor = gl_Color;
	gl_Position = gl_Vertex * modelMatrix * viewMatrix * projectionMatrix;
})"