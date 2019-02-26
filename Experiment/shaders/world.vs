R"(#version 130

uniform mat4 viewMatrix, projectionMatrix;
uniform mat4 mapView[4], mapProjection[4];

out vec4 inMapSpace[4];

void main() {
	gl_FrontColor = gl_Color;
	gl_Position = projectionMatrix * viewMatrix * gl_Vertex;
	for(int i=0; i<4; i++)
		inMapSpace[i] = mapProjection[i] * mapView[i] * gl_Vertex;
})"