R"(#version 130

uniform mat4 modelMatrix, viewMatrix, projectionMatrix;
uniform mat4 mapViews[4], mapProjections[4];

out vec4 inMapSpace[4];

void main() {
	gl_Position = gl_Vertex * modelMatrix * viewMatrix * projectionMatrix;
	for(int i=0; i<4; i++)
		inMapSpace[i] = gl_Vertex * mapViews[i] * mapProjections[i];
})"