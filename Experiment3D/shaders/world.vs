R"(#version 130

uniform mat4 modelMatrix, viewMatrix, projectionMatrix;
uniform mat4 mapViews[4], mapProjections[4];

out vec4 inMapSpace[4];

void main() {
	gl_FrontColor = gl_Color;
	gl_Position = projectionMatrix * viewMatrix * gl_Vertex * modelMatrix;
	for(int i=0; i<4; i++)
		inMapSpace[i] = mapProjections[i] * mapViews[i] * gl_Vertex;
})"